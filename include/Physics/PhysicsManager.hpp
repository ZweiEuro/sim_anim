#pragma once
#include <thread>
#include <allegro5/allegro.h>

namespace mg8
{
  class PhysicsManager
  {
  private:
    static PhysicsManager *m_instance;
    ALLEGRO_EVENT_QUEUE *m_PhysicsManager_event_queue = nullptr;
    ALLEGRO_TIMER *m_physics_refresh_timer = nullptr;

    std::thread m_collision_check_thread;

    void physics_loop();
    PhysicsManager();

  public:
    static PhysicsManager *instance();

    std::thread *get_thread()
    {
      return &m_collision_check_thread;
    }
  };
}