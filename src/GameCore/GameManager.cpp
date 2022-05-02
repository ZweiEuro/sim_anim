#include "GameCore/GameManager.hpp"
#include "Rendering/Renderer.hpp"
#include "Input/InputManager.hpp"
#include "Physics/PhysicsManager.hpp"
#include "enums.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <cstring>
#include <spdlog/spdlog.h>

// Game objects
#include "GameObjects/Ball.hpp"

namespace mg8
{

  GameManager *GameManager::m_instance = nullptr;
  ALLEGRO_EVENT_QUEUE *GameManager::m_GameManager_event_queue = nullptr;

  ALLEGRO_EVENT_SOURCE GameManager::m_GameManager_event_source_to_InputManager;
  ALLEGRO_EVENT_SOURCE GameManager::m_GameManager_event_source_to_Renderer;
  ALLEGRO_EVENT_SOURCE GameManager::m_GameManager_event_source_to_PhysicsManager;
  ALLEGRO_EVENT_SOURCE GameManager::m_GameManager_event_source_to_GameManager;

  GameManager *GameManager::instance()
  {
    if (!m_instance)
    {
      initializeAllegro();

      m_instance = new GameManager();
    }
    return m_instance;
  }

  static std::atomic_bool event_sources_created = false;

  GameManager::GameManager()
  {

    // setup program control events, create all source to subsystems
    al_init_user_event_source(&m_GameManager_event_source_to_InputManager);
    al_init_user_event_source(&m_GameManager_event_source_to_Renderer);
    al_init_user_event_source(&m_GameManager_event_source_to_PhysicsManager);
    al_init_user_event_source(&m_GameManager_event_source_to_GameManager);
    event_sources_created = true;

    // gamemanager event queue but do not subscribe to itself
    m_GameManager_event_queue = al_create_event_queue();
    al_register_event_source(m_GameManager_event_queue, &m_GameManager_event_source_to_GameManager);

    // setup renderer
    auto display = Renderer::instance()->get_current_display();
    // subscribe to display events
    al_register_event_source(m_GameManager_event_queue, al_get_display_event_source(display));

    // start input manager
    InputManager::instance();

    // start physics
    PhysicsManager::instance();

    // done
    spdlog::info("Game manager instanced");

    // fluff stuff

    setup_game();
  }

  GameManager::~GameManager()
  {
    spdlog::error("GameManager destroyed? This is a singleton and shouldn't happen.");
  }

  std::vector<GameObject *> &GameManager::getGameObjects(bool exclusive)
  {
    if (exclusive)
    {
      l_game_objects.lock();
    }
    else
    {
      l_game_objects.lock_shared();
    }
    return m_game_objects;
  }

  void GameManager::releaseGameObjects(bool exclusive)
  {
    if (exclusive)
    {
      l_game_objects.unlock();
    }
    else
    {
      l_game_objects.unlock_shared();
    }
  }

  ALLEGRO_EVENT_SOURCE *GameManager::get_GameManager_event_source_to(MG8_SUBSYSTEMS target_system)
  {

    assert(event_sources_created.load() && "Renderer not yet instanced, no event sources");

    ALLEGRO_EVENT_SOURCE *ret = nullptr;

    switch (target_system)
    {
    case MG8_SUBSYSTEMS::GAMEMANAGER:
      ret = &m_GameManager_event_source_to_GameManager;
      break;
    case MG8_SUBSYSTEMS::INPUT_MANAGER:
      ret = &m_GameManager_event_source_to_InputManager;
      break;
    case MG8_SUBSYSTEMS::PHYSICS_MANAGER:
      ret = &m_GameManager_event_source_to_PhysicsManager;
      break;
    case MG8_SUBSYSTEMS::RENDERER:
      ret = &m_GameManager_event_source_to_Renderer;
      break;

    default:
      break;
    }

    return ret;
  }

  void GameManager::loop()
  {
    bool exit = false;

    while (!exit)
    {
      ALLEGRO_EVENT event;

      al_wait_for_event(m_GameManager_event_queue, &event);

      // Handle the event
      switch (event.type)
      {
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        spdlog::info("GM DISPLAY close event");
        exit = true;
        break;
      case ALLEGRO_EVENT_DISPLAY_RESIZE: // handled in renderer
        break;
      case USER_BASE_EVENT:
        switch ((int)event.user.data1)
        {
        case CONTROL_SHUTDOWN:
          exit = true;
          spdlog::info("GM control shutdown");

          break;
        default:
          spdlog::info("GameManager User event unknown subtype: {}", (int)event.user.data1);
          break;
        }
        break;
      default:
        spdlog::info("Gamemanager unsupported event received: {}", event.type);
        break;
      }
    }

    // shutdown renderer
    send_user_event(MG8_SUBSYSTEMS::RENDERER, CONTROL_SHUTDOWN);
    Renderer::instance()->get_thread()->join();

    // shutdown physics
    send_user_event(MG8_SUBSYSTEMS::PHYSICS_MANAGER, CONTROL_SHUTDOWN);
    PhysicsManager::instance()->get_thread()->join();

    // shutdown input
    send_user_event(MG8_SUBSYSTEMS::INPUT_MANAGER, CONTROL_SHUTDOWN);
    InputManager::instance()->get_thread()->join();
  }
  void GameManager::send_user_event(MG8_SUBSYSTEMS target_system, MG8_EVENTS event)
  {
    ALLEGRO_EVENT ev = {};
    ev.user.data1 = (int)event;
    ev.type = USER_BASE_EVENT;

    if (!al_emit_user_event(get_GameManager_event_source_to(target_system), &ev, nullptr))
    {
      spdlog::warn("Event {} would not have been received by system {}", event, target_system);
    }
  }

  void GameManager::setup_game()
  {
    std::thread([=]() -> void
                {
                  InputManager::instance()->wait_for_key(ALLEGRO_KEY_ESCAPE);
                  send_user_event(MG8_SUBSYSTEMS::GAMEMANAGER, CONTROL_SHUTDOWN); })
        .detach();

    std::thread([=]() -> void
                {
                    while(true) {
                      vec2i pos;
                      vec2i dir;

                      auto ok = InputManager::instance()->wait_for_mouse_button(1, pos);
                      if(!ok){
                        return;
                      }

                      ok = InputManager::instance()->wait_for_mouse_button(1, dir);
                      if (!ok)
                      {
                        return;
                      }
                      auto &objects = getGameObjects(true);
                      objects.emplace_back(new Ball(MG8_OBJECT_TYPES::TYPE_BILIARD_BALL, pos, (dir - pos).dir() * 100, 10));
                      releaseGameObjects(true);
                  } })
        .detach();

    auto &objects = getGameObjects(true);

    // ball moving right
    // not moving
    objects.emplace_back(new Ball(MG8_OBJECT_TYPES::TYPE_BILIARD_BALL, {(float)config_start_resolution_w / 2.0f * 1.5f, (float)config_start_resolution_h / 2.0f}, {0, 0}, 10));
    releaseGameObjects(true);
  }

  bool GameManager::initializeAllegro()
  {
    // Initialize Allegro
    assert(al_init() && "al_init failed");
    assert(al_init_image_addon() && "al_init_image_addon failed");
    assert(al_init_font_addon() && "al_init_image_addon failed");
    assert(al_init_ttf_addon() && "al_init_ttf_addon failed");
    assert(al_init_primitives_addon() && "al_init_primitives_addon failed");
    assert(al_install_mouse() && "al_install_mouse failed");
    assert(al_install_keyboard() && "al_install_keyboard failed");

    return true;
  }

}
