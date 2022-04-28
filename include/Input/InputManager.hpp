#pragma once
#include <allegro5/allegro.h>
#include <mutex>
#include <thread>
#include "math/vector.hpp"

namespace mg8
{

  class InputManager
  {
  private:
    static InputManager *m_instance;
    ALLEGRO_EVENT_QUEUE *m_InputManager_event_queue; // main queue for the GameManager

    ALLEGRO_EVENT_SOURCE m_InputManager_event_source; // where the manager tells "wait for keys" if something was pressed or not
    vec2 m_mouse_state;
    std::mutex l_mouse_state;

    std::thread m_input_thread;

    InputManager();

    void update_mouse_pos();
    void input_loop();

  public:
    static InputManager *instance();

    vec2 get_mouse_position();

    std::thread *get_thread()
    {
      return &m_input_thread;
    }

    void wait_for_key(int keycode); // block until key is pressed
    void wait_for_mouse_button(int button);
  };
}