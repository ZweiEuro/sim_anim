#pragma once
#include "configuration.hpp"
#include "GameObject/GameObject.hpp"
namespace mg8
{

  class GameManager
  {
  private:
    static GameManager *_instance;
    GameObject _game_objects[config_max_object_count]; // set to only 0 (nullptr) when initialized

    GameManager();

  public:
    static GameManager *instance();

    void operator=(GameManager const &) = delete;
    GameManager(GameManager const &) = delete;
  };

}