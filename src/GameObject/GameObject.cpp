
#include "GameObject/GameObject.hpp"
#include "math/vector.hpp"
#include "configuration.hpp"

#include <spdlog/spdlog.h>
namespace mg8
{

  void GameObject::CheckObjectCreation()
  {
    m_objects_created++;
    if (m_objects_created >= (int)config_max_object_count)
    {
      spdlog::warn("%d Objects are currently active, maximum is %d",
                   m_objects_created, config_max_object_count);
    }
  }

  GameObject::GameObject(const vec2 coords)
  {
    m_coords = coords;
  }
  GameObject::GameObject(const float x, const float y)
  {
    m_coords.x = x;
    m_coords.y = y;
  }

  GameObject::~GameObject()
  {
    m_objects_created--;
  }
}