#pragma once

#include "math/vector.hpp"

namespace mg8
{

  class GameObject
  {
  public:
    vec2 _coords; // Object coordiantes in 2D space

    GameObject(const vec2 coords);
    GameObject(const float x = 0, const float y = 0);
  };

}
