#include "Physics/PhysicsManager.hpp"
#include "GameCore/GameManager.hpp"

#include <spdlog/spdlog.h>
#include <chrono>
namespace mg8
{

  PhysicsManager *PhysicsManager::m_instance = nullptr;

  PhysicsManager *PhysicsManager::instance()
  {
    if (!m_instance)
    {
      m_instance = new PhysicsManager();
    }
    return m_instance;
  }

  PhysicsManager::PhysicsManager()
  {
    assert(!m_collision_check_thread.joinable());

    m_PhysicsManager_event_queue = al_create_event_queue();

    m_physics_refresh_timer = al_create_timer(1.0 / config_physics_updates_per_second);
    if (!m_physics_refresh_timer)
    {
      spdlog::error("Could not create physics refresh timer.");
      abort();
    }

    al_register_event_source(m_PhysicsManager_event_queue, GameManager::get_GameManager_event_source_to(MG8_SUBSYSTEMS::PHYSICS_MANAGER));
    al_register_event_source(m_PhysicsManager_event_queue, al_get_timer_event_source(m_physics_refresh_timer));

    al_start_timer(m_physics_refresh_timer);

    m_collision_check_thread = std::thread([=]() -> void
                                           { this->check_collision(); });
  }

  void PhysicsManager::check_collision()
  { // Display a black screen

    bool exit = false;

    while (!exit)
    {
      ALLEGRO_EVENT event;
      static bool recalculate = true;

      static double delta_ms = 0;

      {
        auto start = std::chrono::high_resolution_clock::now();
        al_wait_for_event(m_PhysicsManager_event_queue, &event);
        auto end = std::chrono::high_resolution_clock::now();

        delta_ms = std::chrono::duration<double, std::milli>(end - start).count() / 1000; // why is chrono like this -.-
      }

      // Handle the event

      switch (event.type)
      {
      case ALLEGRO_EVENT_TIMER:
        recalculate = true;
        break;
      case USER_BASE_EVENT:
        switch ((int)event.user.data1)
        {

        case CONTROL_SHUTDOWN:
          exit = true;
          break;
        default:
          spdlog::info("[Physics] User event subtype: {}", (int)event.user.data1);
          break;
        }
        break;
      default:
        spdlog::info("[Physics] event received: {}", event.type);
        break;
      }

      if (recalculate)
      {
        recalculate = false;
        auto objects = GameManager::instance()->getGameObjects();
        // movement resolve
        for (auto &A : objects)
        {
          A->move(A->m_velocity * delta_ms);
        }

        // Collision resolve
        for (auto &A : objects)
          for (auto &B : objects)
          {
            if (A == B)
            {
              continue;
            }
            if (A->collides_with(B))
              spdlog::info("collision");
          }

        // do collision stuff here

        GameManager::instance()->releaseGameObjects();
      }
    }
    spdlog::info("[Physics] exitted");
    return;
  }
}