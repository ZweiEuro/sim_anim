#include "GameCore/PhysicsManager.hpp"
#include "GameCore/GameManager.hpp"
#include "Rendering/Renderer.hpp"
#include "GameObjects/Ball.hpp"
#include "GameObjects/RigidBody.hpp"
#include "GameObjects/Hole.hpp"
#include "GameObjects/ParticleDynamics.hpp"

#include "Rendering/SettingsGui.hpp"

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

    m_physics_refresh_timer = al_create_timer(1.0 / config_default_pps);
    if (!m_physics_refresh_timer)
    {
      spdlog::error("Could not create physics refresh timer.");
      abort();
    }

    al_register_event_source(m_PhysicsManager_event_queue, GameManager::get_GameManager_event_source_to(MG8_SUBSYSTEMS::PHYSICS_MANAGER));
    al_register_event_source(m_PhysicsManager_event_queue, al_get_timer_event_source(m_physics_refresh_timer));

    al_start_timer(m_physics_refresh_timer);
    running = true;

    m_collision_check_thread = std::thread([=]() -> void
                                           { this->physics_loop(); });

    for (int w = 0; w < int(config_start_resolution_w / config_forcefield_grid_dims) + 1; w++)
    {
      m_forcefield.push_back({});

      for (int h = 0; h < int(config_start_resolution_h / config_forcefield_grid_dims) + 1; h++)
      {
        m_forcefield[w].push_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_BALL,
                                                MG8_GAMEOBJECT_TYPES::TYPE_PLAYER1_BALL,
                                                {w * config_forcefield_grid_dims, h * config_forcefield_grid_dims},
                                                {0, 0},
                                                3,
                                                {0.0f, 0.0f},
                                                1,
                                                0,
                                                {255, 255, 255, 255}));
      }
    }
  }

  void PhysicsManager::physics_loop()
  { // Display a black screen

    bool exit = false;

    while (!exit)
    {
      ALLEGRO_EVENT event;
      static bool recalculate = true;

      static double delta_ms = 0;

      {
        delta_time_start_point = std::chrono::high_resolution_clock::now();
        al_wait_for_event(m_PhysicsManager_event_queue, &event);
        auto end = std::chrono::high_resolution_clock::now();

        delta_ms = std::chrono::duration<double, std::milli>(end - delta_time_start_point).count() / 1000; // why is chrono like this -.-
        delta_ms *= SettingsGUI::instance()->m_time_delta_permultiplier_value.load();
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
        auto &objects = GameManager::instance()->getGameObjects();

        l_forcefield.lock();
        for (auto &row : m_forcefield)
        {
          for (auto &point : row)
          {
            point->m_velocity = {0, 0};
          }
        }

        // first all the gravity

        for (auto &A : objects)
        {
          if (A->m_type == TYPE_GRAVITY_WELL)
          {
            const auto *grav = dynamic_cast<const GravityWell *>(A);
            assert(grav && "Grav well cast failed");

            if ((std::time(nullptr) - grav->creation_time) > gravity_well_despawn_time)
            {
              spdlog::info("erasing old gravity well -> age > {} seconds", gravity_well_despawn_time);
              objects.erase(std::remove(objects.begin(), objects.end(), A), objects.end());
              continue;
            }

            grav->apply(objects, delta_ms);

            for (auto &row : m_forcefield)
            {

              grav->apply(row, delta_ms);
            }
          }
        }
        l_forcefield.unlock();

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

            if (collide(A, B))
            {
            }
          }

        // do collision stuff here

        GameManager::instance()->releaseGameObjects();
      }
    }
    spdlog::info("[Physics] exitted");
    return;
  }

  bool PhysicsManager::collide(GameObject *&A, GameObject *&B)
  {

    {
      auto p = dynamic_cast<RigidBody *>(B);
      auto h = dynamic_cast<Hole *>(B);
      if (p)
      {
        if (A->collides_with(p))
        {
          auto p_A = dynamic_cast<RigidBody *>(A);
          p_A->handle_collision(p);
          // spdlog::info("collision");
          return true;
        }
      }
      else if (h)
      {
        if (A->collides_with(h))
        {
          // auto a = dynamic_cast<RigidBody *>(A);
          h->handle_collision(A);
          // spdlog::info("hole collision");
          return true;
        }
      }
    }
    return false;
  }

  void PhysicsManager::pause()
  {
    assert(m_physics_refresh_timer && "No timer to pause for physics");
    if (running.exchange(false))
    {
      spdlog::info("[Physics] paused");
      al_stop_timer(m_physics_refresh_timer);
    }
  }
  void PhysicsManager::resume()
  {
    assert(m_physics_refresh_timer && "No timer to resume for physics");
    if (!running.exchange(true))
    {
      spdlog::info("[Physics] resumed");
      delta_time_start_point = std::chrono::high_resolution_clock::now();
      al_start_timer(m_physics_refresh_timer);
    }
  }

  void PhysicsManager::toggle()
  {

    if (running.load())
    {
      pause();
    }
    else
    {
      resume();
    }
  }
}