#include "Rendering/Renderer.hpp"
#include "GameCore/GameManager.hpp"
#include "util/scope_guard.hpp"
#include "util/file.hpp"
#include "configuration.hpp"
#include "enums.hpp"

#include <spdlog/spdlog.h>
#include <allegro5/allegro_primitives.h>
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

  void Renderer::draw_table()
  {
    /*
     *  image coodinate axis
     * (0,0)---------------> x-axis
     *     |
     *     |
     *     |
     *     |
     *     v
     *     y-axis
     */

    float hole_radius = 10.0;
    float table_border_width = 20;

    float outer_border_x_offset = 20;
    float inner_border_x_offset = outer_border_x_offset + table_border_width;

    float pool_table_width = (float)config_start_resolution_w - 2 * outer_border_x_offset;
    float pool_table_height = pool_table_width / 2.0;

    float outer_border_y_offset = ((float)config_start_resolution_h - pool_table_height) / 2.0;
    float inner_border_y_offset = outer_border_y_offset + table_border_width;

    _TestBilliardTable.left_border_pos = inner_border_x_offset + _TestBall.radius / 2;
    _TestBilliardTable.right_border_pos = (float)config_start_resolution_w - inner_border_x_offset - _TestBall.radius / 2;
    _TestBilliardTable.upper_border_pos = inner_border_y_offset + _TestBall.radius / 2;
    _TestBilliardTable.lower_border_pos = (float)config_start_resolution_h - inner_border_y_offset - _TestBall.radius / 2;

    // outer brown border of table
    al_draw_filled_rounded_rectangle(outer_border_x_offset, outer_border_y_offset, (float)config_start_resolution_w - outer_border_x_offset,
                                     (float)config_start_resolution_h - outer_border_y_offset, hole_radius, hole_radius, al_map_rgb(102, 51, 0));

    // inner green field of the table
    al_draw_filled_rectangle(inner_border_x_offset, inner_border_y_offset, (float)config_start_resolution_w - inner_border_x_offset,
                             (float)config_start_resolution_h - inner_border_y_offset, al_map_rgb(0, 102, 0));

    // left upper hole
    al_draw_filled_circle(inner_border_x_offset + hole_radius / 4, inner_border_y_offset + hole_radius / 4, hole_radius, al_map_rgb(0, 0, 0));

    // left lower hole
    al_draw_filled_circle(inner_border_x_offset + hole_radius / 4, (float)config_start_resolution_h - inner_border_y_offset - hole_radius / 4, hole_radius, al_map_rgb(0, 0, 0));

    // right upper hole
    al_draw_filled_circle((float)config_start_resolution_w - inner_border_x_offset - hole_radius / 4, inner_border_y_offset + hole_radius / 4, hole_radius, al_map_rgb(0, 0, 0));

    // right lower hole
    al_draw_filled_circle((float)config_start_resolution_w - inner_border_x_offset - hole_radius / 4, (float)config_start_resolution_h - inner_border_y_offset - hole_radius / 4, hole_radius, al_map_rgb(0, 0, 0));

    // center upper hole
    al_draw_filled_circle((float)config_start_resolution_w / 2.0, inner_border_y_offset, hole_radius, al_map_rgb(0, 0, 0));

    // center lower hole
    al_draw_filled_circle((float)config_start_resolution_w / 2.0, (float)config_start_resolution_h - inner_border_y_offset, hole_radius, al_map_rgb(0, 0, 0));
  };

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

    // enable anti-aliasing
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

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

    if (!al_init_image_addon())
    {
      spdlog::error("Failed to init image addon.");
      abort();
    }

    if (!al_init_primitives_addon())
    {
      spdlog::error("Failed to init primitives addon.");
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
      static bool redraw = true;

      al_wait_for_event(m_renderer_event_queue, &event);

      // Handle the event

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
      // the al_is_empty is not needed anymore since get_event is false if the timeout triggered
      if (redraw) //&& al_is_event_queue_empty(m_renderer_event_queue))
      {
        redraw = false;

        // Redraw
        /*  _TestBall.x += _TestBall.dx;
          _TestBall.y += _TestBall.dy;

          if (_TestBall.x < _TestBilliardTable.left_border_pos)
          {
            _TestBall.x = _TestBilliardTable.left_border_pos + (_TestBilliardTable.left_border_pos - _TestBall.x);
            _TestBall.dx *= -1;
          }
          if (_TestBall.x > _TestBilliardTable.right_border_pos)
          {
            _TestBall.x = _TestBilliardTable.right_border_pos - (_TestBall.x - _TestBilliardTable.right_border_pos);
            _TestBall.dx *= -1;
          }
          if (_TestBall.y < _TestBilliardTable.upper_border_pos)
          {
            _TestBall.y = _TestBilliardTable.upper_border_pos + (_TestBilliardTable.upper_border_pos - _TestBall.y);
            _TestBall.dy *= -1;
          }
          if (_TestBall.y > _TestBilliardTable.lower_border_pos)
          {
            _TestBall.y = _TestBilliardTable.lower_border_pos - (_TestBall.y - _TestBilliardTable.lower_border_pos);
            _TestBall.dy *= -1;
          }        */

        al_clear_to_color(al_map_rgb(100, 0, 0));
        // draw_table();
        // al_draw_filled_circle(_TestBall.x, _TestBall.y, _TestBall.radius, al_map_rgb(0, 0, 255));
        auto objects = GameManager::instance()->getGameObjects();
        //  spdlog::info("redraw {} objects", objects->size());

        for (const auto &obj : *objects)
        {
          obj->draw();
        }

        al_flip_display();
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
