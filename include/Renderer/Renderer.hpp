#pragma once

#include <stdio.h>
#include <allegro5/allegro.h>
#include <thread>
#include <atomic>
#include <mutex>

namespace mg8
{

  class Renderer
  {
  private:
    static Renderer *m_instance;

    ALLEGRO_DISPLAY *m_display = nullptr;
    ALLEGRO_EVENT_QUEUE *m_renderer_event_queue = nullptr; // [NOTE] this is destryoed on unsetup, maybe this needs to be changed so its only instanced once.
    ALLEGRO_TIMER *m_display_refresh_timer = nullptr;

    std::thread m_rendering_thread;        // created _once_ and then never again or only reused. the events from the gamemanager queu can be used to restart it cleanly without needing to manage a new thread
    std::mutex m_rendering_resources_lock; // control all rendering resources and only give it free when they all are assigned and valid, if they are invalid the lock is taken

    void setup(); // called first by the main render thread
    void unsetup();
    Renderer();

  public:
    static Renderer *instance();
    ~Renderer();

    void stop_rendering();
    void render_loop();

    ALLEGRO_DISPLAY *get_current_display(); // guarded
  };

}