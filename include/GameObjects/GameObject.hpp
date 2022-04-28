#pragma once

#include "math/math.hpp"
#include <atomic>

namespace mg8
{

  enum MG8_OBJECT_TYPES : uint32_t
  {
    TYPE_UNDEFINED = 0,
    TYPE_BILIARD_BALL = 1 << 0,
    TYPE_TABLE_HOLE = 1 << 1
  };

  enum MG8_PHYSICS_PROPERTY
  {
    PHYS_UNDEFINED = 0,
    PHYS_PERFCETLY_ELASTIC,
    PHYS_UNACCELERATABLE // immovable + static unchanging movement
  };

  class GameObject
  {
  public:
    vec2 m_velocity = {0, 0}; // Object direction of movement in 3D space

    MG8_OBJECT_TYPES m_type = TYPE_UNDEFINED;
    uint32_t m_collision_mask = TYPE_UNDEFINED; // & of object types

    virtual void draw() const = 0;

    GameObject(const MG8_OBJECT_TYPES type,
               const uint32_t collision = 0,
               const vec2 velocity = {0, 0});

    ~GameObject();
  };

}
