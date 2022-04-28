
#include "GameObjects/GameObject.hpp"
#include "math/math.hpp"
#include "configuration.hpp"

#include <spdlog/spdlog.h>
namespace mg8
{

  GameObject::GameObject(const MG8_OBJECT_TYPES type,
                         const uint32_t collision,
                         const vec2 velocity) : m_type(type),
                                                m_collision_mask(collision),
                                                m_velocity(velocity)
  {
  }

  GameObject::~GameObject()
  {
  }
}