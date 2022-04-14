#pragma once

#include "math/vector.hpp"

namespace mg8
{

  class GameObject
  {
  private:
    int m_objects_created = 0;

    void CheckObjectCreation();

  public:
    vec2 m_coords; // Object coordiantes in 2D space

    GameObject(const vec2 coords);
    GameObject(const float x = 0, const float y = 0);

    ~GameObject();
  };

}
