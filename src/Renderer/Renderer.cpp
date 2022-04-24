#include "Renderer/Renderer.hpp"
#include "GameManager/GameManager.hpp"
#include "util/scope_guard.hpp"
#include "util/file.hpp"
#include "configuration.hpp"
#include "enums.hpp"

#include <spdlog/spdlog.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
namespace mg8
{
  Renderer *Renderer::m_instance = nullptr;

  Renderer *Renderer::instance()
  {
    if (!m_instance)
    {
      m_instance = new Renderer();
    }

    return m_instance;
  }

  Renderer::~Renderer()
  {
    spdlog::error("Renderer destroyed? This is a singleton and shouldn't happen.");
  }

  Renderer::Renderer()
  {
    spdlog::info("Renderer instanced");

    assert(!m_rendering_thread.joinable() && "rendering thread exists but init was called ?");
    m_rendering_resources_lock.lock();

    this->m_rendering_thread = std::thread([=]() -> void
                                           {
      this->setup(); // thread objects are automatically friend so private fct can be called
      this->render_loop(); });
  }

  void Renderer::unsetup()
  {
    spdlog::info("Renderer unsetupped");
    m_rendering_resources_lock.lock();

    al_stop_timer(m_display_refresh_timer);

    al_destroy_timer(m_display_refresh_timer);
    m_display_refresh_timer = nullptr;
    al_destroy_event_queue(m_renderer_event_queue);
    m_renderer_event_queue = nullptr;
    al_destroy_display(m_display);
    m_display = nullptr;
  }

  void Renderer::setup()
  {
    assert(!m_display && "Rendering setup but display already allocated");
    assert(!m_display_refresh_timer && "Rendering setup but Timer already allocated");
    assert(!m_renderer_event_queue && "Rendering setup but display_event_queue already allocated");

    spdlog::info("Renderer setupped");

    m_display_refresh_timer = al_create_timer(1.0 / config_fps);
    if (!m_display_refresh_timer)
    {
      spdlog::error("Could not create display refresh timer.");
      abort();
    }

    m_display = al_create_display(config_start_resolution_w, config_start_resolution_h);
    if (!m_display)
    {
      spdlog::error("Failed to create display.");
      abort();
    }

    m_renderer_event_queue = al_create_event_queue();
    if (!m_renderer_event_queue)
    {
      spdlog::error("Failed to create display event queue.");
      abort();
    }

    al_register_event_source(m_renderer_event_queue, al_get_timer_event_source(m_display_refresh_timer));
    al_register_event_source(m_renderer_event_queue, GameManager::get_GameManager_event_source_to(MG8_SUBSYSTEMS::RENDERER));

    al_clear_to_color(al_map_rgb(0, 0, 0)); // initial clear
    al_flip_display();
    m_rendering_resources_lock.unlock();

    al_start_timer(m_display_refresh_timer);
  }

  void Renderer::render_loop()
  {
    // Display a black screen

    bool exit = false;

    while (!exit)
    {
      ALLEGRO_EVENT event;
      ALLEGRO_TIMEOUT timeout;
      static bool redraw = true;

      // Initialize timeout
      al_init_timeout(&timeout, 0.06);

      // Fetch the event (if one exists)
      bool get_event = al_wait_for_event_until(m_renderer_event_queue, &event, &timeout);

      // Handle the event
      if (get_event)
      {
        switch (event.type)
        {
        case ALLEGRO_EVENT_TIMER:
          redraw = true;
          break;
        case USER_BASE_EVENT:
          switch ((int)event.user.data1)
          {
          case CONTROL_SHUTDOWN:
            exit = true;
            break;
          default:
            spdlog::info("Renderer Unknown User event subtype: {}", (int)event.user.data1);

            break;
          }

          break;
        default:
          spdlog::info("Renderer unknown event received: {}", event.type);
          break;
        }
      }
      else
      {             // the al_is_empty is not needed anymore since get_event is false if the timeout triggered
        if (redraw) //&& al_is_event_queue_empty(m_renderer_event_queue))
        {
          spdlog::info("redraw");
          // Redraw
          al_clear_to_color(al_map_rgb(100, 0, 0));
          al_flip_display();
          redraw = false;
        }
      }

      // Check if we need to redraw, only redraw if the queue is now empty
    }
    unsetup();
    spdlog::info("render thread exitted");
    return;
  }

  ALLEGRO_DISPLAY *Renderer::get_current_display()
  {
    auto guard = ScopeGuard(m_rendering_resources_lock);
    return m_display;
  }
}
