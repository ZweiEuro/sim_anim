#pragma once
#include "configuration.hpp"
#include "GameObject/GameObject.hpp"
#include "util/guard_ptr.hpp"

#include <mutex>
namespace mg8
{

  class GameManager
  {
  private:
    static GameManager *m_instance;

    std::mutex l_game_objects;
    GameObject m_game_objects[config_max_object_count]; // set to only 0 (nullptr) when initialized

    GameManager();

  public:
    static GameManager *instance();

    void operator=(GameManager const &) = delete;
    GameManager(GameManager const &) = delete;

    mutex_guard_ptr<GameObject> getGameObjects(); // locks internally, needs to be unlocked
    void unlock_GameObjects();
  };

}