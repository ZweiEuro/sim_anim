#include "Input/InputManager.hpp"

#include "GameCore/GameManager.hpp"
#include "configuration.hpp"
#include "util/scope_guard.hpp"

#include <spdlog/spdlog.h>

namespace mg8
{
  InputManager *InputManager::m_instance = nullptr;

  InputManager *InputManager::instance()
  {
    if (!m_instance)
    {
      m_instance = new InputManager();
    }
    return m_instance;
  }

  InputManager::InputManager()
  {
    // get communication from gamemanager

    assert(al_install_mouse() && "could not install mouse driver");
    assert(al_install_keyboard() && "could not install keyboard driver");

    assert(!m_input_thread.joinable());

    m_InputManager_event_queue = al_create_event_queue();

    al_init_user_event_source(&m_InputManager_event_source);

    al_register_event_source(m_InputManager_event_queue, GameManager::get_GameManager_event_source_to(MG8_SUBSYSTEMS::INPUT_MANAGER));
    al_register_event_source(m_InputManager_event_queue, al_get_mouse_event_source());
    al_register_event_source(m_InputManager_event_queue, al_get_keyboard_event_source());

    m_input_thread = std::thread([=]() -> void
                                 { this->input_loop(); });
  }

  void InputManager::input_loop()
  {

    // Display a black screen

    bool exit = false;

    while (!exit)
    {
      ALLEGRO_EVENT event;

      // Fetch the event (if one exists)
      al_wait_for_event(m_InputManager_event_queue, &event);

      // Handle the event

      switch (event.type)
      {
      case ALLEGRO_EVENT_MOUSE_AXES:

        l_mouse_state.lock();
        m_mouse_state.x = event.mouse.x;
        m_mouse_state.y = event.mouse.y;
        l_mouse_state.unlock();

        break;
      case ALLEGRO_EVENT_KEY_DOWN:
        spdlog::info("key pressed {}", al_keycode_to_name(event.keyboard.keycode));
        al_emit_user_event(&m_InputManager_event_source, &event, nullptr);
        break;
      case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        spdlog::info("mouse pressed pressed Button {}({} > 2 = other) @ {} {}", event.mouse.button == 1 ? "left" : "right", event.mouse.button, event.mouse.x, event.mouse.y);
        al_emit_user_event(&m_InputManager_event_source, &event, nullptr);
        break;
      case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
      case ALLEGRO_EVENT_KEY_CHAR:
      case ALLEGRO_EVENT_KEY_UP:
        // ignore all type events or key up events
        break;
      case USER_BASE_EVENT:
        switch ((int)event.user.data1)
        {
        case CONTROL_SHUTDOWN:
          exit = true;
          break;
        default:
          spdlog::info("Input thread User event subtype: {}", (int)event.user.data1);
          break;
        }
        break;
      default:
        spdlog::info("Input thread event received: {}", event.type);
        break;
      }
    }
    spdlog::info("Input thread exitted");
    return;
  }

  vec2 InputManager::get_mouse_position()
  {
    l_mouse_state.lock();
    auto ret = m_mouse_state;
    l_mouse_state.unlock();
    return ret;
  }

  void InputManager::wait_for_key(int keycode)
  {
    auto queue = al_create_event_queue();
    al_register_event_source(queue, &m_InputManager_event_source);
    bool exit = false;
    while (!exit)
    {
      ALLEGRO_EVENT event;
      al_wait_for_event(queue, &event);
      if (event.type == ALLEGRO_EVENT_KEY_DOWN)
      {
        exit = (event.keyboard.keycode == keycode);
      }
    }
    al_unregister_event_source(queue, &m_InputManager_event_source);
    al_destroy_event_queue(queue);
  }

  vec2 InputManager::wait_for_mouse_button(int button)
  {
    auto queue = al_create_event_queue();
    al_register_event_source(queue, &m_InputManager_event_source);
    bool exit = false;
    vec2 ret = {};
    while (!exit)
    {
      ALLEGRO_EVENT event;
      al_wait_for_event(queue, &event);
      if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
      {
        exit = (event.mouse.button & button);
        if (exit)
        {
          ret = {event.mouse.x, event.mouse.y};
        }
      }
    }
    al_unregister_event_source(queue, &m_InputManager_event_source);
    al_destroy_event_queue(queue);

    return ret;
  }
}