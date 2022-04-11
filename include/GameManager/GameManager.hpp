#pragma once

namespace mg8
{

  class GameManager
  {
  private:
    static GameManager *_instance;

    GameManager();

  public:
    static GameManager *instance();

    void operator=(GameManager const &) = delete;
    GameManager(GameManager const &) = delete;
  };

}