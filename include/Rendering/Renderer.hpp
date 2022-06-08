#pragma once

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include <Agui/Agui.hpp>
#include <Agui/Backends/Allegro5/Allegro5.hpp>

#include "configuration.hpp"
#include <thread>
#include <atomic>
#include <mutex>

namespace mg8
{

  struct TestBall
  {
    float x = (float)config_start_resolution_w / 2.0f;
    float y = (float)config_start_resolution_h / 2.0f;
    float dx = 10;
    float dy = 10;
    float radius = 9;
  };

  struct TestBilliardTable
  {
    float left_border_pos = 0;
    float right_border_pos = (float)config_start_resolution_w;
    float upper_border_pos = 0;
    float lower_border_pos = (float)config_start_resolution_h;
  };

  class Renderer
  {
  private:
    static Renderer *m_instance;

    ALLEGRO_DISPLAY *m_display = nullptr;
    ALLEGRO_EVENT_QUEUE *m_renderer_event_queue = nullptr; // [NOTE] this is destryoed on unsetup, maybe this needs to be changed so its only instanced once.
    ALLEGRO_TIMER *m_display_refresh_timer = nullptr;

    std::thread m_rendering_thread;        // created _once_ and then never again or only reused. the events from the gamemanager queu can be used to restart it cleanly without needing to manage a new thread
    std::mutex m_rendering_resources_lock; // control all rendering resources and only give it free when they all are assigned and valid, if they are invalid the lock is taken

    // display properties
    int m_display_width = config_start_resolution_w;
    int m_display_height = config_start_resolution_h;

    // GUI shizzle
    agui::Allegro5Input *m_inputHandler = NULL;
    agui::Allegro5Graphics *m_graphicsHandler = NULL;

    std::vector<agui::Gui *> m_guiComponents;
    void renderGUI();

    // functions
    void setup(); // called first by the main render thread
    void render_loop();
    void unsetup();
    void draw_scoreboard();
    Renderer();

    // testing stuff

    TestBall _TestBall;
    TestBilliardTable _TestBilliardTable;

  public:
    static Renderer *instance();
    ~Renderer();

    void stop_rendering();

    // getter and setter
    ALLEGRO_DISPLAY *get_current_display(); // guarded
    std::thread *get_thread()
    {
      return &m_rendering_thread;
    }
    agui::Allegro5Input *get_agui_input_handler()
    {
      assert(m_inputHandler);
      return m_inputHandler;
    }
    agui::Allegro5Graphics *get_agui_graphics_handler()
    {
      assert(m_graphicsHandler);
      return m_graphicsHandler;
    }

    bool mouse_hit_widget()
    {
      for (auto &gui : m_guiComponents)
      {
        if (gui->isWidgetUnderMouse())
          return true;
      }
      return false;
    }

    void setFPS(float FPS)
    {
      al_set_timer_speed(m_display_refresh_timer, 1.0 / FPS);
    }

    void logicGUI();
    void resizeGUI();

    std::atomic<bool> render_settings = true;
  };

}