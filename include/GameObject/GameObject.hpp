#pragma once

#include "math/vector.hpp"
#include <atomic>

namespace mg8
{

  class GameObject
  {
  private:
    static std::atomic_int m_objects_created;

    void CheckObjectCreation();

  public:
    vec2 m_coords; // Object coordiantes in 2D space

    GameObject(const vec2 coords);
    GameObject(const float x = 0, const float y = 0);

    ~GameObject();
  };

}
