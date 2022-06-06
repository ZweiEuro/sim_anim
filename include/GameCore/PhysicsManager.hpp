#pragma once
#include <thread>
#include <allegro5/allegro.h>
#include <atomic>
#include "GameObjects/RigidBody.hpp"

namespace mg8
{
  class GameObject;

  class PhysicsManager
  {
  private:
    static PhysicsManager *m_instance;
    ALLEGRO_EVENT_QUEUE *m_PhysicsManager_event_queue = nullptr;
    ALLEGRO_TIMER *m_physics_refresh_timer = nullptr;

    std::thread m_collision_check_thread;
    std::atomic<bool> running = false;

    void physics_loop();
    PhysicsManager();
    bool collide(GameObject *&A, GameObject *&B);

  protected:
    std::chrono::_V2::system_clock::time_point delta_time_start_point;

  public:
    static PhysicsManager *
    instance();

    std::thread *get_thread()
    {
      return &m_collision_check_thread;
    }

    void pause();
    void resume();
    void toggle();

    void setPPS(float PPS)
    {

      al_set_timer_speed(m_physics_refresh_timer, 1.0 / PPS);
    }

    std::vector<std::vector<GameObject *>> m_forcefield;
  };

}