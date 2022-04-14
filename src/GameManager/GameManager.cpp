#include "GameManager/GameManager.hpp"

#include <cstring>

namespace mg8
{
  GameManager *GameManager::_instance = nullptr;

  GameManager *GameManager::instance()
  {
    if (!_instance)
    {
      _instance = new GameManager();
    }
    return _instance;
  }
  GameManager::GameManager()
  {
    std::memset((void *)_game_objects, 0, config_max_object_count * sizeof(GameObject *));

    // spdlog::info("Game manager instanced");
  }
}