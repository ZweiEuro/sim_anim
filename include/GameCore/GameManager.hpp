#pragma once
#include "configuration.hpp"
#include "GameObjects/GameObject.hpp"
#include "util/guard_ptr.hpp"
#include "enums.hpp"

#include <allegro5/allegro.h>
#include <Agui/Font.hpp>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <memory>

#include "math/PathInterpol.hpp"

namespace mg8
{

  class GameManager
  {
  private:
    static GameManager *m_instance;

    static bool instanced;

    std::shared_timed_mutex l_game_objects;
    std::vector<GameObject *> m_game_objects = {};
    GameObject *m_white_ball = nullptr;

    bool player_one_active = false;
    bool player_two_active = false;

    // Events need to be fired to each system from a new source so they don't starve each other
    static ALLEGRO_EVENT_SOURCE m_GameManager_event_source_to_InputManager; // the queue the input thread listens to
    static ALLEGRO_EVENT_SOURCE m_GameManager_event_source_to_Renderer;     // The renderer queue also gets forwarded most display events for processing, but stuff like "exit button pressed" will need to be managed by the GameManager first for controlled shutdown
    static ALLEGRO_EVENT_SOURCE m_GameManager_event_source_to_PhysicsManager;
    static ALLEGRO_EVENT_SOURCE m_GameManager_event_source_to_GameManager; // the GameManager queue if another thread needs to tell it something, handles display events first

    // this should always exist so other systems can register it, if it isn't by the time its asked the getter will fail
    static ALLEGRO_EVENT_QUEUE *m_GameManager_event_queue; // main queue for the GameManager

    agui::Font *defaultFont = nullptr;

    GameManager();

    void spawnGame();

  public:
    bool objects_moving = false;

    bool debug_enabled = false;
    bool voronoi_recalc = false;

    splineCurve *curve = nullptr;

    static bool initializeAllegro();
    static GameManager *instance();

    void operator=(GameManager const &) = delete;
    GameManager(GameManager const &) = delete;
    ~GameManager();

    std::vector<GameObject *> &getGameObjects(bool exclusive = false); // locks internally, needs to be unlocked
    void releaseGameObjects(bool exclusive = false);

    void loop(); // main management loop, this is where the main ends up in
    void send_user_event(MG8_SUBSYSTEMS target_system, MG8_EVENTS event);

    // Fails if get source is not yet valid
    static ALLEGRO_EVENT_SOURCE *get_GameManager_event_source_to(MG8_SUBSYSTEMS target_system);

    void setup_game();
  };

}