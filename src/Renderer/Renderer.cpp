#include "Renderer/Renderer.hpp"
#include "configuration.hpp"

#include <spdlog/spdlog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include <string>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

namespace mg8
{

  Renderer *Renderer::m_instance = nullptr;

  Renderer *Renderer::instance()
  {
    if (!m_instance)
    {
      // very round about way to make sure the renderer thread is the one constructing all the renderer stuff that is needed

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
  }

  void Renderer::start_rendering()
  {
    if (m_rendering_thread != nullptr)
    {
      spdlog::warn("rendering started but already running in thread");
      return;
    }

    m_instance->m_rendering_thread = new std::thread([]() -> void
                                                     { Renderer::instance()->render_loop(); });
  }

  void Renderer::stop_rendering()
  {

    if (m_rendering_thread == nullptr)
    {
      spdlog::warn("Rendering not running anywhere");
      return;
    }

    m_rendering_thread->join();
  }
  void Renderer::unsetup()
  {
    spdlog::info("Renderer unsetupped");

    al_destroy_timer(m_display_refresh_timer);
    m_display_refresh_timer = nullptr;
    al_destroy_event_queue(m_display_event_queue);
    m_display_event_queue = nullptr;
    al_destroy_display(m_display);
    m_display = nullptr;
  }

  void Renderer::setup()
  {
    spdlog::info("Renderer setupped");

    m_display_refresh_timer = al_create_timer(1.0 / config_fps);
    if (!m_display_refresh_timer)
    {
      spdlog::error("Could not create display refresh timer.");
      abort();
    }

    //enable anti-aliasing
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    m_display = al_create_display(config_start_resolution_w, config_start_resolution_h);
    if (!m_display)
    {
      spdlog::error("Failed to create display.");
      abort();
    }

    m_display_event_queue = al_create_event_queue();
    if (!m_display_event_queue)
    {
      spdlog::error("Failed to create display event queue.");
      abort();
    }

    if(!al_init_image_addon())
    {
      spdlog::error("Failed to init image addon.");
      abort();
    }
    table = al_load_bitmap("billiard.bmp");
    if (!al_init_primitives_addon())
    {
      spdlog::error("Failed to init primitives addon.");
      abort();
    }

    al_register_event_source(m_display_event_queue, al_get_display_event_source(m_display));
    al_register_event_source(m_display_event_queue, al_get_timer_event_source(m_display_refresh_timer));
  }

  void Renderer::render_loop()
  {
    static std::atomic_bool running = false; // should only be running once or we're gonna have weird drawing artefacts

    //ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
    //al_append_path_component(path, "..");
    //al_set_path_filename(path, "billiard.bmp");
    //ALLEGRO_BITMAP* table = al_load_bitmap(al_path_cstr(path, '/'));
    
    if (!table)
    {
      //std::string path = "../../img";
      //for (const auto & entry : fs::directory_iterator(path))
      //    std::cout << entry.path() << std::endl;
      spdlog::error("Failed to load the billiard table top view image.");
      //spdlog::error(al_path_cstr(path, '/'));
      abort();
    }
    

    if (running.exchange(true))
    {
      spdlog::error("Two threads are inside the render loop!");
      abort();
    }
    else
    {
      setup();
      // Display a black screen
      al_clear_to_color(al_map_rgb(0, 0, 0));
      al_flip_display();

      // Start the timer
      al_start_timer(m_display_refresh_timer);
    }

    while (m_render.load())
    {
      ALLEGRO_EVENT event;
      ALLEGRO_TIMEOUT timeout;
      static bool redraw = true;

      // Initialize timeout
      al_init_timeout(&timeout, 0.06);

      // Fetch the event (if one exists)
      bool get_event = al_wait_for_event_until(m_display_event_queue, &event, &timeout);

      // Handle the event
      if (get_event)
      {
        switch (event.type)
        {
        case ALLEGRO_EVENT_TIMER:
          redraw = true;
          break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
          spdlog::info("Display close event");
          running = false;
          break;
        default:
          spdlog::info("Unsupported Display received: %d", event.type);
          break;
        }
      }

      // Check if we need to redraw, only redraw if the queue is now empty
      if (redraw && al_is_event_queue_empty(m_display_event_queue))
      {
        // Redraw
        al_clear_to_color(al_map_rgb(0, 0, 0));

        //draw image -> billiard table
        al_draw_bitmap(table, 100, 100, 0);
        //try to let a circle bounce around within the image of the billiard table
        al_draw_circle(450, 370, 30, al_map_rgb_f(1, 0, 1), 2);
        al_flip_display();
        redraw = false;
      }
    }
    spdlog::info("render thread exitted");

    running = false;
    return;
  }

}
