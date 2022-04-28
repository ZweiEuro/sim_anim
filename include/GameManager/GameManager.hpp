#pragma once
#include "configuration.hpp"
#include "GameObject/GameObject.hpp"
#include "util/guard_ptr.hpp"
#include "enums.hpp"

#include <allegro5/allegro.h>
#include <mutex>
#include <thread>
#include <vector>
#include <memory>

namespace mg8
{

  class GameManager
  {
  private:
    static GameManager *m_instance;

    std::mutex l_game_objects;
    std::vector<std::shared_ptr<GameObject>> m_game_objects = {};

    // Events need to be fired to each system from a new source so they don't starve each other
    static ALLEGRO_EVENT_SOURCE m_GameManager_event_source_to_InputManager; // the queue the input thread listens to
    static ALLEGRO_EVENT_SOURCE m_GameManager_event_source_to_Renderer;     // The renderer queue also gets forwarded most display events for processing, but stuff like "exit button pressed" will need to be managed by the GameManager first for controlled shutdown
    static ALLEGRO_EVENT_SOURCE m_GameManager_event_source_to_PhysicsManager;
    static ALLEGRO_EVENT_SOURCE m_GameManager_event_source_to_GameManager; // the GameManager queue if another thread needs to tell it something, handles display events first

    // this should always exist so other systems can register it, if it isn't by the time its asked the getter will fail
    static ALLEGRO_EVENT_QUEUE *m_GameManager_event_queue; // main queue for the GameManager

    GameManager();

    // fluff, not really necessary

    std::thread escape_button_listener;

  public:
    static GameManager *instance();

    void operator=(GameManager const &) = delete;
    GameManager(GameManager const &) = delete;
    ~GameManager();

    mutex_guard_ptr<std::vector<std::shared_ptr<GameObject>>> getGameObjects(); // locks internally, needs to be unlocked

    void loop(); // main management loop, this is where the main ends up in
    void send_user_event(MG8_SUBSYSTEMS target_system, MG8_EVENTS event);

    // Fails if get source is not yet valid
    static ALLEGRO_EVENT_SOURCE *get_GameManager_event_source_to(MG8_SUBSYSTEMS target_system);

    void setup_game();
    };

}