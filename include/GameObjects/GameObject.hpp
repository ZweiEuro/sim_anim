#pragma once

#include "math/vector.hpp"
#include <atomic>

namespace mg8
{

  enum MG8_OBJECT_TYPES : uint32_t
  {
    UNDEFINED = 0,
    BALL = 1 << 0,
    BOX = 1 << 1
  };

  class GameObject
  {
  public:
    vec2 m_position = {0, 0}; // Object coordiantes in 2D space
    vec2 m_velocity = {0, 0}; // Object direction of movement in 3D space

    MG8_OBJECT_TYPES m_type = UNDEFINED;
    uint32_t m_collision_mask = UNDEFINED; // & of object types

    virtual void draw() const = 0;

    GameObject(const MG8_OBJECT_TYPES type,
               const uint32_t collision = 0,
               const vec2 position = {0, 0},
               const vec2 velocity = {0, 0});

    ~GameObject();
  };

}
