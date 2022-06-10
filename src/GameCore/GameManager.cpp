#include "GameCore/GameManager.hpp"
#include "Rendering/Renderer.hpp"
#include "Input/InputManager.hpp"
#include "GameCore/PhysicsManager.hpp"
#include "Rendering/SettingsGui.hpp"

#include "enums.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "allegro5/allegro_native_dialog.h"

#include <cstring>
#include <spdlog/spdlog.h>

// Game objects
#include "GameObjects/Ball.hpp"
#include "GameObjects/RigidBody.hpp"
#include "GameObjects/Hole.hpp"
#include "GameObjects/ParticleDynamics.hpp"

#include "math/VoronoiFracture.hpp"

namespace mg8
{

  GameManager *GameManager::m_instance = nullptr;
  bool GameManager::instanced = false;
  ALLEGRO_EVENT_QUEUE *GameManager::m_GameManager_event_queue = nullptr;

  ALLEGRO_EVENT_SOURCE GameManager::m_GameManager_event_source_to_InputManager;
  ALLEGRO_EVENT_SOURCE GameManager::m_GameManager_event_source_to_Renderer;
  ALLEGRO_EVENT_SOURCE GameManager::m_GameManager_event_source_to_PhysicsManager;
  ALLEGRO_EVENT_SOURCE GameManager::m_GameManager_event_source_to_GameManager;

  GameManager *GameManager::instance()
  {
    if (!instanced)
    {
      instanced = true;
      initializeAllegro();
      spdlog::info("init GameManager");
      m_instance = new GameManager();
    }
    return m_instance;
  }

  static std::atomic_bool event_sources_created = false;

  GameManager::GameManager()
  {

    // setup static system wide stuff

    agui::Image::setImageLoader(new agui::Allegro5ImageLoader);
    agui::Font::setFontLoader(new agui::Allegro5FontLoader);

    defaultFont = agui::Font::load("res/fonts/DejaVuSans.ttf", 16);
    assert(defaultFont && "Default font failed to load");
    agui::Widget::setGlobalFont(defaultFont);

    // setup program control events, create all source to subsystems
    al_init_user_event_source(&m_GameManager_event_source_to_InputManager);
    al_init_user_event_source(&m_GameManager_event_source_to_Renderer);
    al_init_user_event_source(&m_GameManager_event_source_to_PhysicsManager);
    al_init_user_event_source(&m_GameManager_event_source_to_GameManager);
    event_sources_created = true;

    // gamemanager event queue but do not subscribe to itself
    m_GameManager_event_queue = al_create_event_queue();
    al_register_event_source(m_GameManager_event_queue, &m_GameManager_event_source_to_GameManager);

    // setup renderer
    auto display = Renderer::instance()->get_current_display();
    // subscribe to display events
    al_register_event_source(m_GameManager_event_queue, al_get_display_event_source(display));

    // start input manager
    InputManager::instance();

    // start physics
    PhysicsManager::instance();

    // done
    spdlog::info("Game manager instanced");

    setup_game();
  }

  GameManager::~GameManager()
  {
    spdlog::error("GameManager destroyed? This is a singleton and shouldn't happen.");
  }

  std::vector<GameObject *> &GameManager::getGameObjects(bool exclusive)
  {
    if (exclusive)
    {
      l_game_objects.lock();
    }
    else
    {
      l_game_objects.lock_shared();
    }
    return m_game_objects;
  }

  void GameManager::releaseGameObjects(bool exclusive)
  {
    if (exclusive)
    {
      l_game_objects.unlock();
    }
    else
    {
      l_game_objects.unlock_shared();
    }
  }

  ALLEGRO_EVENT_SOURCE *GameManager::get_GameManager_event_source_to(MG8_SUBSYSTEMS target_system)
  {

    assert(event_sources_created.load() && "Renderer not yet instanced, no event sources");

    ALLEGRO_EVENT_SOURCE *ret = nullptr;

    switch (target_system)
    {
    case MG8_SUBSYSTEMS::GAMEMANAGER:
      ret = &m_GameManager_event_source_to_GameManager;
      break;
    case MG8_SUBSYSTEMS::INPUT_MANAGER:
      ret = &m_GameManager_event_source_to_InputManager;
      break;
    case MG8_SUBSYSTEMS::PHYSICS_MANAGER:
      ret = &m_GameManager_event_source_to_PhysicsManager;
      break;
    case MG8_SUBSYSTEMS::RENDERER:
      ret = &m_GameManager_event_source_to_Renderer;
      break;

    default:
      break;
    }

    return ret;
  }

  void GameManager::loop()
  {
    bool exit = false;

    while (!exit)
    {
      ALLEGRO_EVENT event;

      al_wait_for_event(m_GameManager_event_queue, &event);

      // Handle the event
      switch (event.type)
      {
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        spdlog::info("GM DISPLAY close event");
        exit = true;
        break;
      case ALLEGRO_EVENT_DISPLAY_RESIZE: // handled in renderer
        break;
      case USER_BASE_EVENT:
        switch ((int)event.user.data1)
        {
        case CONTROL_SHUTDOWN:
          exit = true;
          spdlog::info("GM control shutdown");

          break;
        default:
          spdlog::info("GameManager User event unknown subtype: {}", (int)event.user.data1);
          break;
        }
        break;
      default:
        spdlog::info("Gamemanager unsupported event received: {}", event.type);
        break;
      }
    }

    // shutdown renderer
    send_user_event(MG8_SUBSYSTEMS::RENDERER, CONTROL_SHUTDOWN);
    spdlog::warn("Waiting for rendering");
    // Renderer::instance()->get_thread()->join();

    // shutdown physics
    send_user_event(MG8_SUBSYSTEMS::PHYSICS_MANAGER, CONTROL_SHUTDOWN);
    spdlog::warn("Waiting for physics");

    PhysicsManager::instance()->get_thread()->join();

    // shutdown input
    send_user_event(MG8_SUBSYSTEMS::INPUT_MANAGER, CONTROL_SHUTDOWN);
    spdlog::warn("Waiting for control");

    InputManager::instance()->get_thread()->join();
  }
  void GameManager::send_user_event(MG8_SUBSYSTEMS target_system, MG8_EVENTS event)
  {
    ALLEGRO_EVENT ev = {};
    ev.user.data1 = (int)event;
    ev.type = USER_BASE_EVENT;

    if (!al_emit_user_event(get_GameManager_event_source_to(target_system), &ev, nullptr))
    {
      spdlog::warn("Event {} would not have been received by system {}", event, target_system);
    }
  }

  void GameManager::setup_game()
  {

    // escape exit
    std::thread([=]() -> void
                {
                  InputManager::instance()->wait_for_key(ALLEGRO_KEY_ESCAPE);
                  send_user_event(MG8_SUBSYSTEMS::GAMEMANAGER, CONTROL_SHUTDOWN);

                  exit(0); })
        .detach();

    // Toggle GUI
    std::thread([=]() -> void
                {
                  while (true)
                  {
                    auto ok = InputManager::instance()->wait_for_key(ALLEGRO_KEY_S);
                    if(!ok)
                    return;
                    Renderer::instance()->render_settings = !Renderer::instance()->render_settings;
                  } })
        .detach();

    // reset game
    std::thread([=]() -> void
                {
                    while(true) {

                      if(!InputManager::instance()->wait_for_key(ALLEGRO_KEY_R)){
                        return;
                      }
                      spawnGame();
                  } })
        .detach();

    // Pause physics
    std::thread([=]() -> void
                {
                  while (true)
                  {
                    auto ok = InputManager::instance()->wait_for_key(ALLEGRO_KEY_P);
                    if(!ok)
                    return;

                    
                    PhysicsManager::instance()->toggle();

                  } })
        .detach();

    // gravity well spawner
    std::thread([=]() -> void
                {
                  while (true)
                  {
                    mg8::vec2i pos;
                    auto ok = InputManager::instance()->wait_for_mouse_button(2, pos);
                    if (!ok)
                      return;
                    auto &objects = getGameObjects(true);

                    auto found =   std::find_if(objects.begin(), objects.end(),[](const GameObject* x) { return x->m_type == TYPE_GRAVITY_WELL;});

                    int count =0;
                    for(auto c = found ; c != objects.end();c++)
                    {
                      count++;
                    }

                    if (count >= config_max_number_grav_wells)
                    {
                      objects.erase(std::remove(objects.begin(), objects.end(), found[0]), objects.end());
                    }

                    objects.emplace_back(new GravityWell(MG8_OBJECT_TYPES::TYPE_GRAVITY_WELL,pos,{0, 0}, 10));
                    releaseGameObjects(true);
                  } })
        .detach();

    // control white ball
    std::thread([=]() -> void
                {
                    while(true) {
                      
                        vec2i dir;
                        if (m_white_ball && m_white_ball->m_velocity.mag() == 0)
                        {

                          auto white_ball = dynamic_cast<RigidBody *>(m_white_ball);
                          auto ok = InputManager::instance()->wait_for_mouse_button(1, dir);
                          if(!ok){
                            return;
                          }
                          white_ball->m_velocity = (dir - white_ball->circle::pos).dir() * (dir - white_ball->circle::pos).mag() * (SettingsGUI::instance()->m_white_ball_power_value.load() / 10.f);
                          spdlog::info("white ball velocity x: {}, y: {}", white_ball->m_velocity.x, white_ball->m_velocity.y);
                          player1_active = !player1_active;
                        }                      
                  } })
        .detach();

    spawnGame();
  }

  bool GameManager::initializeAllegro()
  {
    // Initialize Allegro
    assert(al_init() && "al_init failed");
    assert(al_init_image_addon() && "al_init_image_addon failed");
    assert(al_init_font_addon() && "al_init_image_addon failed");
    assert(al_init_ttf_addon() && "al_init_ttf_addon failed");
    assert(al_init_primitives_addon() && "al_init_primitives_addon failed");
    assert(al_install_mouse() && "al_install_mouse failed");
    assert(al_install_keyboard() && "al_install_keyboard failed");
    assert(al_init_native_dialog_addon() && "al_init_native_dialog_addon failed");

    return true;
  }

  void GameManager::spawnGame()
  {

    auto &objects = getGameObjects(true);
    objects.clear();

    // left border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {outer_border_x_offset, inner_border_y_offset + hole_radius * 2}, {0, 0}, table_border_width, (float)config_start_resolution_h - (inner_border_y_offset + hole_radius * 2) * 2, 0.0f, CENTER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    // upper border 1
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {inner_border_x_offset + hole_radius * 2, outer_border_y_offset}, {0, 0}, ((float)config_start_resolution_w - (inner_border_x_offset + hole_radius * 2) * 2) / 2 - 3 * hole_radius, table_border_width, 0.0f, CENTER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    // upper border 2
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {inner_border_x_offset + hole_radius * 2 + ((float)config_start_resolution_w - (inner_border_x_offset + hole_radius * 2) * 2) / 2 + 3 * hole_radius, outer_border_y_offset}, {0, 0}, ((float)config_start_resolution_w - (inner_border_x_offset + hole_radius * 2) * 2) / 2 - 3 * hole_radius, table_border_width, 0.0f, CENTER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    // right border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {(float)config_start_resolution_w - inner_border_x_offset, inner_border_y_offset + hole_radius * 2}, {0, 0}, table_border_width, (float)config_start_resolution_h - (inner_border_y_offset + hole_radius * 2) * 2, 0.0f, CENTER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    // lower border 1
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {inner_border_x_offset + hole_radius * 2, (float)config_start_resolution_h - inner_border_y_offset}, {0, 0}, ((float)config_start_resolution_w - (inner_border_x_offset + hole_radius * 2) * 2) / 2 - 3 * hole_radius, table_border_width, 0.0f, CENTER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    // lower border 2
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {inner_border_x_offset + hole_radius * 2 + ((float)config_start_resolution_w - (inner_border_x_offset + hole_radius * 2) * 2) / 2 + 3 * hole_radius, (float)config_start_resolution_h - inner_border_y_offset}, {0, 0}, ((float)config_start_resolution_w - (inner_border_x_offset + hole_radius * 2) * 2) / 2 - 3 * hole_radius, table_border_width, 0.0f, CENTER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));

    // left upper hole - upper border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {inner_border_x_offset + hole_radius * 2 - hole_radius * 4, inner_border_y_offset - table_border_width}, {0, 0}, hole_radius * 4, table_border_width, 45.0f, RIGHT_LOWER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    // left upper hole - lower border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {inner_border_x_offset - hole_radius * 4, inner_border_y_offset + hole_radius * 2}, {0, 0}, hole_radius * 4, table_border_width, 45.0f, RIGHT_UPPER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));

    // right upper hole - upper border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {(float)config_start_resolution_w - inner_border_x_offset - hole_radius * 2, inner_border_y_offset - table_border_width}, {0, 0}, hole_radius * 4, table_border_width, -45.0f, LEFT_LOWER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    // right upper hole - lower border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {(float)config_start_resolution_w - inner_border_x_offset, inner_border_y_offset + hole_radius * 2}, {0, 0}, hole_radius * 4, table_border_width, -45.0f, LEFT_UPPER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));

    // right lower hole - upper border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {(float)config_start_resolution_w - inner_border_x_offset, (float)config_start_resolution_h - inner_border_y_offset - hole_radius * 2 - table_border_width}, {0, 0}, hole_radius * 4, table_border_width, 45.0f, LEFT_LOWER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    // right lower hole - lower border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {(float)config_start_resolution_w - inner_border_x_offset - hole_radius * 2, (float)config_start_resolution_h - inner_border_y_offset}, {0, 0}, hole_radius * 4, table_border_width, 45.0f, LEFT_UPPER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));

    // left lower hole - upper border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {inner_border_x_offset - hole_radius * 4, (float)config_start_resolution_h - inner_border_y_offset - hole_radius * 2 - table_border_width}, {0, 0}, hole_radius * 4, table_border_width, -45.0f, RIGHT_LOWER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    // left lower hole - lower border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {inner_border_x_offset + hole_radius * 2 - hole_radius * 4, (float)config_start_resolution_h - inner_border_y_offset}, {0, 0}, hole_radius * 4, table_border_width, -45.0f, RIGHT_UPPER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));

    // middle upper hole - left border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {(float)config_start_resolution_w / 2.0f - 3 * hole_radius, inner_border_y_offset - table_border_width}, {0, 0}, hole_radius * 4, table_border_width, -45.0f, LEFT_LOWER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    // middle upper hole - right border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {(float)config_start_resolution_w / 2.0f + 3 * hole_radius, inner_border_y_offset}, {0, 0}, hole_radius * 4, table_border_width, -135.0f, LEFT_UPPER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));

    // middle lower hole - left border
    // objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {(float)config_start_resolution_w / 2.0f - 3 * hole_radius, (float)config_start_resolution_h - inner_border_y_offset}, {0, 0}, hole_radius * 4, table_border_width, 45.0f, LEFT_UPPER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {(float)config_start_resolution_w / 2.0f - 3 * hole_radius - table_border_width, (float)config_start_resolution_h - inner_border_y_offset}, {0, 0}, hole_radius * 4, table_border_width, -45.0f, RIGHT_UPPER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));

    // middle lower hole - right border
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_TABLE_BORDER, {(float)config_start_resolution_w / 2.0f + 3 * hole_radius, (float)config_start_resolution_h - inner_border_y_offset - table_border_width}, {0, 0}, hole_radius * 4, table_border_width, 135.0f, LEFT_LOWER_CORNER, {0.0f, 0.0f}, 1.0f, 0.8f, al_map_rgb(102, 51, 0)));

    // left upper hole
    objects.emplace_back(new Hole(MG8_OBJECT_TYPES::TYPE_TABLE_HOLE, {inner_border_x_offset - hole_radius, inner_border_y_offset - hole_radius}, {0, 0}, sqrtf(powf(hole_radius, 2) + powf(hole_radius, 2)) - 0.2));
    // middle upper hole
    objects.emplace_back(new Hole(MG8_OBJECT_TYPES::TYPE_TABLE_HOLE, {(float)config_start_resolution_w / 2.0f, inner_border_y_offset - hole_radius}, {0, 0}, sqrtf(powf(hole_radius, 2) + powf(hole_radius, 2)) - 0.2));
    // right upper hole
    objects.emplace_back(new Hole(MG8_OBJECT_TYPES::TYPE_TABLE_HOLE, {(float)config_start_resolution_w - inner_border_x_offset + hole_radius, inner_border_y_offset - hole_radius}, {0, 0}, sqrtf(powf(hole_radius, 2) + powf(hole_radius, 2)) - 0.2));
    // left lower hole
    objects.emplace_back(new Hole(MG8_OBJECT_TYPES::TYPE_TABLE_HOLE, {inner_border_x_offset - hole_radius, (float)config_start_resolution_h - inner_border_y_offset + hole_radius}, {0, 0}, sqrtf(powf(hole_radius, 2) + powf(hole_radius, 2)) - 0.2));
    // middle lower hole
    objects.emplace_back(new Hole(MG8_OBJECT_TYPES::TYPE_TABLE_HOLE, {(float)config_start_resolution_w / 2.0f, (float)config_start_resolution_h - inner_border_y_offset + hole_radius}, {0, 0}, sqrtf(powf(hole_radius, 2) + powf(hole_radius, 2)) - 0.2));
    // right lower hole
    objects.emplace_back(new Hole(MG8_OBJECT_TYPES::TYPE_TABLE_HOLE, {(float)config_start_resolution_w - inner_border_x_offset + hole_radius, (float)config_start_resolution_h - inner_border_y_offset + hole_radius}, {0, 0}, sqrtf(powf(hole_radius, 2) + powf(hole_radius, 2)) - 0.2));

    // Rotated test rectangle
    // objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_OBSTACLE_RECTANGLE, {((float)config_start_resolution_w - (inner_border_x_offset + hole_radius * 2) * 2) / 2 + 3 * hole_radius + 6 * hole_radius, (float)config_start_resolution_h / 2}, {0, 0}, hole_radius * 4, table_border_width / 2, 45.0f, LEFT_LOWER_CORNER, {0.0f, 0.0f}, 1.0f, 0.6f, al_map_rgb(0, 0, 0)));

    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_RECTANGLE, MG8_GAMEOBJECT_TYPES::TYPE_ICE_RECTANGLE, {(float)inner_border_x_offset * 2, (float)config_start_resolution_h / 2}, {0, 0}, 200, table_border_width * 3, 45.0f, LEFT_LOWER_CORNER, {0.0f, 0.0f}, 1.0f, 0.85f, al_map_rgb(100, 220, 220)));
    // rect r = rect({(float)inner_border_x_offset * 2, (float)config_start_resolution_h / 2}, 200, table_border_width * 3, 45.0f, LEFT_LOWER_CORNER);
    // VoronoiFracture *v = new VoronoiFracture(&r);

    // auto p = dynamic_cast<RigidBody *>(objects.back());
    // p->v = v;

    float center_offset_x = 50;
    float center_offset_y = 50;
    float center_x = (float)config_start_resolution_w / 2;
    float center_y = (float)config_start_resolution_h / 2;
    /*curve = new splineCurve();
    curve->addControlPoint(vec2f(center_x - center_offset_x, center_y - center_offset_y));
    curve->addControlPoint(vec2f(center_x + center_offset_x * 2, center_y + center_offset_y));
    curve->addControlPoint(vec2f(center_x + center_offset_x * 2, center_y - center_offset_y));

    curve->addControlPoint(vec2f(center_x - center_offset_x, center_y + center_offset_y));
    // curve->addControlPoint(vec2f(center_x - center_offset_x / 2, center_y + center_offset_y / 2));
    curve->calcCurve();

    objects.back()->movementCurve = curve;
    */
    // ball moving right
    // not moving
    // objects.emplace_back(new Ball(MG8_OBJECT_TYPES::TYPE_BALL, {(float)config_start_resolution_w / 2.0f * 1.5f, (float)config_start_resolution_h / 2.0f}, {0, 0}, 10));

    // objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_BALL, MG8_GAMEOBJECT_TYPES::TYPE_PLAYER1_BALL, {(float)config_start_resolution_w / 2.0f * 1.2f, (float)config_start_resolution_h / 2.0f}, {0, 0}, 10, {0.0f, 0.0f}, 0.2f, 0.93, {0, 0, 255, 255}));
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_BALL, MG8_GAMEOBJECT_TYPES::TYPE_WHITE_BALL, {(float)config_start_resolution_w / 2.0f * 1.5f, (float)config_start_resolution_h / 2.0f}, {0, 0}, 10, {0.0f, 0.0f}, 0.2f, 0.93, {255, 255, 255, 255}));
    m_white_ball = objects.back(); // set white ball
    s = new SceneHierarchy(m_white_ball);
    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_BALL, MG8_GAMEOBJECT_TYPES::TYPE_SATELLITE_BALL, {(float)config_start_resolution_w / 2.0f * 1.5f - 55, (float)config_start_resolution_h / 2.0f}, {0, 0}, 6, {0.0f, 0.0f}, 0.2f, 0.93, {255, 255, 255, 255}));
    std::vector<GameObject *> tmp;
    tmp.push_back(objects.back());
    s->addSatellites(m_white_ball, tmp); // add L1

    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_BALL, MG8_GAMEOBJECT_TYPES::TYPE_SATELLITE_BALL, {(float)config_start_resolution_w / 2.0f * 1.5f - 80, (float)config_start_resolution_h / 2.0f}, {0, 0}, 4.5, {0.0f, 0.0f}, 0.2f, 0.93, {255, 255, 255, 255}));
    std::vector<GameObject *> tmp_;
    tmp_.push_back(objects.back());
    s->addSatellites(tmp[0], tmp_); // add L2

    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_BALL, MG8_GAMEOBJECT_TYPES::TYPE_SATELLITE_BALL, {(float)config_start_resolution_w / 2.0f * 1.5f - 95, (float)config_start_resolution_h / 2.0f}, {0, 0}, 2.5, {0.0f, 0.0f}, 0.2f, 0.93, {255, 255, 255, 255}));
    std::vector<GameObject *> tmp__;
    tmp__.push_back(objects.back());
    s->addSatellites(tmp_[0], tmp__); // add L3

    // objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_BALL, MG8_GAMEOBJECT_TYPES::TYPE_BLACK_BALL, {(float)config_start_resolution_w / 2.0f * 1.0f, (float)config_start_resolution_h / 2.0f}, {0, 0}, 10, {0.0f, 0.0f}, 0.2f, 0.93, {0, 0, 0, 255}));
    for (int i = 0; i < 5; i++)
    {
      objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_BALL, MG8_GAMEOBJECT_TYPES::TYPE_PLAYER1_BALL, vec2f((float)config_start_resolution_w / 2.0f * 1.2f, (float)config_start_resolution_h / 2.0f) + vec2f(-30 * i, 15), {0, 0}, 10, {0.0f, 0.0f}, 0.2f, 0.93, {255, 180, 0, 255}));
      objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_BALL, MG8_GAMEOBJECT_TYPES::TYPE_PLAYER2_BALL, vec2f((float)config_start_resolution_w / 2.0f * 1.2f, (float)config_start_resolution_h / 2.0f) + vec2f(-30 * i, -15), {0, 0}, 10, {0.0f, 0.0f}, 0.2f, 0.93, {255, 0, 255, 255}));
    }

    objects.emplace_back(new RigidBody(MG8_RIGID_BODY_OBJECT_TYPES::TYPE_BALL, MG8_GAMEOBJECT_TYPES::TYPE_BLACK_BALL, vec2f((float)config_start_resolution_w / 2.0f * 1.2f, (float)config_start_resolution_h / 2.0f) + vec2f(-30 * 5, 0), {0, 0}, 10, {0.0f, 0.0f}, 0.2f, 0.93, {0, 0, 0, 255}));

    player1_ball_count = 5;
    player2_ball_count = 5;

    releaseGameObjects(true);
  }

}
