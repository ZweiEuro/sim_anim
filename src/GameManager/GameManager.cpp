#include "GameManager/GameManager.hpp"
#include "Renderer/Renderer.hpp"
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

  GameManager::GameManager()
  {

    // memory structure setup
    std::memset((void *)m_game_objects, 0, config_max_object_count * sizeof(GameObject *));

    // setup program control events, create all source to subsystems
    al_init_user_event_source(&m_GameManager_event_source_to_InputManager);
    al_init_user_event_source(&m_GameManager_event_source_to_Renderer);
    al_init_user_event_source(&m_GameManager_event_source_to_PhysicsManager);
    al_init_user_event_source(&m_GameManager_event_source_to_GameManager);

    // gamemanager event queue but do not subscribe to itself
    m_GameManager_event_queue = al_create_event_queue();
    al_register_event_source(m_GameManager_event_queue, &m_GameManager_event_source_to_GameManager);

    // setup renderer
    auto display = Renderer::instance()->get_current_display();
    // subscribe to display events
    al_register_event_source(m_GameManager_event_queue, al_get_display_event_source(display));

    // done
    spdlog::info("Game manager instanced");
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
    ALLEGRO_EVENT_SOURCE *ret = nullptr;

    switch (target_system)
    {
    case MG8_SUBSYSTEMS::GAMEMANAGER:
      ret = &m_GameManager_event_source_to_InputManager;
      break;
    case MG8_SUBSYSTEMS::INPUT_MANAGER:
      ret = &m_GameManager_event_source_to_Renderer;
      break;
    case MG8_SUBSYSTEMS::PHYSICS_MANAGER:
      ret = &m_GameManager_event_source_to_PhysicsManager;
      break;
    case MG8_SUBSYSTEMS::RENDERER:
      ret = &m_GameManager_event_source_to_GameManager;
      break;

    default:
      break;
    }

    return ret;
  }

  void GameManager::loop()
  {

    while (true)
    {
      ALLEGRO_EVENT event;
      ALLEGRO_TIMEOUT timeout;

      al_init_timeout(&timeout, 0.06);

      // Fetch the event (if one exists)
      bool get_event = al_wait_for_event_until(m_GameManager_event_queue, &event, &timeout);

      // Handle the event
      if (get_event)
      {
        switch (event.type)
        {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
          spdlog::info("GameManager Event display closed");

          break;

        case USER_BASE_EVENT:
          spdlog::info("GameManager Event subtype: {}", *(int *)event.user.data1);

          break;
        default:
          spdlog::info("Gamemanager unsupported event received: {}", event.type);
          break;
        }
      }
    }
  }

}