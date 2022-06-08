#include "Rendering/Renderer.hpp"
#include "GameCore/GameManager.hpp"
#include "Rendering/SettingsGui.hpp"
#include "util/scope_guard.hpp"
#include "util/file.hpp"
#include "configuration.hpp"
#include "enums.hpp"

#include <spdlog/spdlog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include <Agui/Agui.hpp>
#include <Agui/Backends/Allegro5/Allegro5.hpp>
#include <Agui/Backends/Allegro5/Allegro5Input.hpp>
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

    // Set the input handler
    agui::Color::setPremultiplyAlpha(true);

    m_inputHandler = new agui::Allegro5Input();
    m_graphicsHandler = new agui::Allegro5Graphics();

    assert(!m_rendering_thread.joinable() && "rendering thread exists but init was called ?");
    m_rendering_resources_lock.lock();

    this->m_rendering_thread = std::thread([=]() -> void
                                           {
      this->setup(); // thread objects are automatically friend so private fct can be called
      this->render_loop(); });
  }

  void Renderer::draw_scoreboard()
  {
    std::string s = "Player 1: BLUE Balls";
    std::string s_ = "Player 2: RED Balls";
    std::string s1 = "Player 1 Ball Count: ";
    s1 = s1 + std::to_string(GameManager::instance()->player1_ball_count);
    std::string s2 = "Player 2 Ball Count: ";
    s2 = s2 + std::to_string(GameManager::instance()->player2_ball_count);
    std::string t1 = "Player 1's turn...";
    std::string t2 = "Player 2's turn...";
    ALLEGRO_FONT *font = al_create_builtin_font();
    al_draw_text(font, al_map_rgb(255, 255, 255), m_display_width * 0.8, 10, 0, s.c_str());
    al_draw_text(font, al_map_rgb(255, 255, 255), m_display_width * 0.8, 25, 0, s_.c_str());
    al_draw_text(font, al_map_rgb(255, 255, 255), m_display_width * 0.8, 45, 0, s1.c_str());
    al_draw_text(font, al_map_rgb(255, 255, 255), m_display_width * 0.8, 60, 0, s2.c_str());
    if (GameManager::instance()->player1_active)
    {
      al_draw_text(font, al_map_rgb(255, 255, 255), m_display_width * 0.8, 75, 0, t1.c_str());
    }
    else
    {
      al_draw_text(font, al_map_rgb(255, 255, 255), m_display_width * 0.8, 75, 0, t2.c_str());
    }
  }

  void Renderer::unsetup()
  {
    m_rendering_resources_lock.lock();

    al_stop_timer(m_display_refresh_timer);

    al_destroy_timer(m_display_refresh_timer);
    m_display_refresh_timer = nullptr;
    al_destroy_event_queue(m_renderer_event_queue);
    m_renderer_event_queue = nullptr;
    al_destroy_display(m_display);
    m_display = nullptr;
    spdlog::info("[Render] unnsetupped");
  }

  void Renderer::setup()
  {
    assert(!m_display && "Rendering setup but display already allocated");
    assert(!m_display_refresh_timer && "Rendering setup but Timer already allocated");
    assert(!m_renderer_event_queue && "Rendering setup but display_event_queue already allocated");

    spdlog::info("Renderer setupped");

    m_display_refresh_timer = al_create_timer(1.0 / config_default_fps);
    if (!m_display_refresh_timer)
    {
      spdlog::error("Could not create display refresh timer.");
      abort();
    }

    // enable anti-aliasing
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    if (config_allow_resize)
    {
      al_set_new_display_flags(ALLEGRO_RESIZABLE);
    }

    m_display = al_create_display(config_start_resolution_w, config_start_resolution_h);
    assert(m_display && "Failed to create display.");

    al_show_mouse_cursor(m_display);
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);

    al_set_window_title(m_display, "Magic 8");

    m_renderer_event_queue = al_create_event_queue();
    assert(m_renderer_event_queue && "Failed to create display event queue.");

    al_register_event_source(m_renderer_event_queue, al_get_timer_event_source(m_display_refresh_timer));
    al_register_event_source(m_renderer_event_queue, GameManager::get_GameManager_event_source_to(MG8_SUBSYSTEMS::RENDERER));
    al_register_event_source(m_renderer_event_queue, al_get_display_event_source(m_display));

    al_clear_to_color(al_map_rgb(0, 0, 0)); // initial clear
    al_flip_display();
    m_rendering_resources_lock.unlock();

    al_start_timer(m_display_refresh_timer);

    m_instance->m_guiComponents.push_back(SettingsGUI::instance());
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
      case ALLEGRO_EVENT_DISPLAY_RESIZE:
        m_display_width = event.display.width;
        m_display_height = event.display.height;
        al_resize_display(m_display, m_display_width, m_display_height);
        al_acknowledge_resize(event.display.source);

        spdlog::info("resized to w: {} h: {}", m_display_width, m_display_height);

        for (auto &gui : m_guiComponents)
        {
          gui->resizeToDisplay();
        }

        redraw = true;
        break;

      case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
      case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
        resizeGUI();
        break;

      case ALLEGRO_EVENT_TIMER:
        redraw = true;
        break;
      case ALLEGRO_EVENT_DISPLAY_CLOSE: // handled in game manager
        break;
      case USER_BASE_EVENT:
        switch ((int)event.user.data1)
        {
        case CONTROL_SHUTDOWN:

          exit = true;
          break;
        default:
          spdlog::info("[Render] Unknown User event subtype: {}", (int)event.user.data1);

          break;
        }

        break;
      default:
        spdlog::info("[Render] unknown event received: {}", event.type);
        break;
      }
      // the al_is_empty is not needed anymore since get_event is false if the timeout triggered
      if (redraw) //&& al_is_event_queue_empty(m_renderer_event_queue))
      {
        redraw = false;

        al_clear_to_color(al_map_rgb(100, 100, 100));
        // al_clear_to_color(al_map_rgb(240, 240, 240));
        al_draw_filled_rounded_rectangle(outer_border_x_offset - table_border_width / 2, outer_border_y_offset - table_border_width / 2, (float)config_start_resolution_w - outer_border_x_offset + table_border_width / 2,
                                         (float)config_start_resolution_h - outer_border_y_offset + table_border_width / 2, hole_radius, hole_radius, al_map_rgb(102, 51, 0));

        // inner green field of the table
        al_draw_filled_rectangle(inner_border_x_offset - 2 * hole_radius, inner_border_y_offset - 2 * hole_radius, (float)config_start_resolution_w - inner_border_x_offset + 2 * hole_radius,
                                 (float)config_start_resolution_h - inner_border_y_offset + 2 * hole_radius, al_map_rgb(0, 102, 0));
        //  draw_table();
        //  al_draw_filled_circle(_TestBall.x, _TestBall.y, _TestBall.radius, al_map_rgb(0, 0, 255));
        auto &objects = GameManager::instance()->getGameObjects();
        //  spdlog::info("redraw {} objects", objects->size());

        for (const auto &obj : objects)
        {
          obj->draw();
        }

        if (SettingsGUI::instance()->m_checkbox_forcefield.checked())
        {
          // draw the forcefield
          PhysicsManager::instance()->l_forcefield.lock();
          for (auto &row : PhysicsManager::instance()->m_forcefield)
          {
            for (auto &point : row)
            {
              auto &body = *dynamic_cast<RigidBody *>(point);

              body.draw();

              auto &circ = *dynamic_cast<circle *>(point);

              auto destination = circ.pos + (body.m_velocity.dir() * 5) * std::min(10.0f, body.m_velocity.mag());

              al_draw_line(circ.pos.x, circ.pos.y, destination.x, destination.y, body.m_color, 2);
            }
          }
          PhysicsManager::instance()->l_forcefield.unlock();
        }

        if (SettingsGUI::instance()->m_checkbox_object_path.checked())
        {
          for (const auto &obj : objects)
          {
            auto p = dynamic_cast<RigidBody *>(obj);
            auto &body = *p;

            if (p && body.m_past_positions.size() > 0)
            {

              auto source = body.m_past_positions.back();
              int count = 0;

              for (auto i = body.m_past_positions.rbegin();
                   i != body.m_past_positions.rend(); ++i)
              {
                auto dest = *i;

                al_draw_line(source.x, source.y, dest.x, dest.y, body.m_color, 2);
                source = *i;

                if (count > config_show_last_X_positions)
                {
                  break;
                }

                count++;
              }
            }
          }
        }

        GameManager::instance()->releaseGameObjects();
        if (GameManager::instance()->debug_enabled)
        {

          GameManager::instance()->curve->drawCurve();
        }
        draw_scoreboard();
        Renderer::instance()->logicGUI();
        renderGUI();
        al_flip_display();
      }

      // Check if we need to redraw, only redraw if the queue is now empty
    }
    unsetup();
    spdlog::info("[Render] exitted");
    return;
  }

  ALLEGRO_DISPLAY *Renderer::get_current_display()
  {
    auto guard = ScopeGuard(m_rendering_resources_lock);
    assert(m_display);

    return m_display;
  }

  void Renderer::renderGUI()
  {
    for (size_t i = 0; i < m_guiComponents.size(); i++)
    {
      if (i == 0 && !render_settings)
        continue;

      auto &gui = m_guiComponents[i];
      gui->render();
    }
  }
  void Renderer::logicGUI()
  {
    for (size_t i = 0; i < m_guiComponents.size(); i++)
    {
      if (i == 0 && !render_settings)
        continue;

      auto &gui = m_guiComponents[i];
      gui->logic();
    }
  }

  void Renderer::resizeGUI()
  {
    for (auto &gui : m_guiComponents)
    {
      gui->resizeToDisplay();
    }
  }

}
