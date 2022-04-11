#include "GameManager/GameManager.hpp"
#include <stdio.h>
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
    printf("Game manager instanced\n");
  }
}