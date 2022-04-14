#include "GameManager/GameManager.hpp"

#include <cstring>
#include <spdlog/spdlog.h>

namespace mg8
{
  GameManager *GameManager::m_instance = nullptr;

  GameManager *GameManager::instance()
  {
    if (!m_instance)
    {
      m_instance = new GameManager();
    }
    return m_instance;
  }

  GameManager::GameManager()
  {
    std::memset((void *)m_game_objects, 0, config_max_object_count * sizeof(GameObject *));
    l_game_exit.lock();

    spdlog::info("Game manager instanced");
  }

  GameManager::~GameManager()
  {
    spdlog::error("GameManager destroyed? This is a singleton and shouldn't happen.");
  }

  mutex_guard_ptr<GameObject> GameManager::getGameObjects()
  {
    return mutex_guard_ptr<GameObject>(l_game_objects, m_game_objects);
  }

  void GameManager::getGameExitPermission()
  {
    l_game_exit.lock();
  }
}