#pragma once

#include <stdio.h>
#include <allegro5/allegro.h>
#include <thread>
#include <atomic>

namespace mg8
{

  class Renderer
  {
  private:
    static Renderer *m_instance;

    ALLEGRO_DISPLAY *m_display = nullptr;
    ALLEGRO_EVENT_QUEUE *m_display_event_queue = nullptr;
    ALLEGRO_TIMER *m_display_refresh_timer = nullptr;

    std::thread *m_rendering_thread = nullptr;
    std::atomic_bool m_render = true; // allow the draw thread to draw or end on next refrash

    void setup(); // called first by the main render thread
    void unsetup();
    Renderer();

  public:
    static Renderer *instance();
    ~Renderer();

    void start_rendering();
    void stop_rendering();
    void render_loop();
  };

}