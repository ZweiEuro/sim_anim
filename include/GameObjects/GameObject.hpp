#pragma once

#include "math/math.hpp"
#include <atomic>

namespace mg8
{

  enum MG8_OBJECT_TYPES : uint32_t
  {
    TYPE_UNDEFINED = 0,
    TYPE_RIGID_BODY = 1 << 0,
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
    vec2f m_velocity = {0, 0}; // Object direction of movement in 2D space

    MG8_OBJECT_TYPES m_type = TYPE_UNDEFINED;
    uint32_t m_collision_mask = TYPE_UNDEFINED; // & of object types

    virtual void draw() const = 0;
    virtual void move(vec2f delta_position) = 0;

    bool collides_with(const GameObject *comp) const;
    bool is_moving() const;
    // virtual void handle_collision(GameObject *collision_with) = 0;

    GameObject(const MG8_OBJECT_TYPES type,
               const uint32_t collision = 0,
               const vec2f velocity = {0, 0});

    ~GameObject();
  };

}
