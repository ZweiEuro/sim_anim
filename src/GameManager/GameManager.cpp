#include "GameManager/GameManager.hpp"
#include "Renderer/Renderer.hpp"
#include "InputManager/InputManager.hpp"
#include "enums.hpp"

#include <allegro5/allegro.h>
#include <cstring>
#include <spdlog/spdlog.h>

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
      m_instance = new GameManager();
    }
    return m_instance;
  }

  static std::atomic_bool event_sources_created = false;

  GameManager::GameManager()
  {

    // memory structure setup
    std::memset((void *)m_game_objects, 0, config_max_object_count * sizeof(GameObject *));

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

    // done
    spdlog::info("Game manager instanced");

    // fluff stuff

    escape_button_listener = std::thread([=]() -> void
                                         {
                                           InputManager::instance()->wait_for_key(ALLEGRO_KEY_ESCAPE);
                                           send_user_event(MG8_SUBSYSTEMS::GAMEMANAGER, CONTROL_SHUTDOWN); });
  }

  GameManager::~GameManager()
  {
    spdlog::error("GameManager destroyed? This is a singleton and shouldn't happen.");
  }

  mutex_guard_ptr<GameObject> GameManager::getGameObjects()
  {
    return mutex_guard_ptr<GameObject>(l_game_objects, m_game_objects);
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
        exit = true;
        break;

      case USER_BASE_EVENT:
        switch ((int)event.user.data1)
        {
        case CONTROL_SHUTDOWN:
          exit = true;
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
}