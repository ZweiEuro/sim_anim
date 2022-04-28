
#include "GameObjects/GameObject.hpp"
#include "math/math.hpp"
#include "configuration.hpp"

#include <spdlog/spdlog.h>
namespace mg8
{

  GameObject::GameObject(const MG8_OBJECT_TYPES type,
                         const uint32_t collision,
                         const vec2 velocity) : m_velocity(velocity),
                                                m_type(type),
                                                m_collision_mask(collision)

  {
  }

  GameObject::~GameObject()
  {
  }

  bool GameObject::collides_with(const GameObject *comp) const
  {
    auto comp_is_circle = dynamic_cast<const circle *>(comp);
    auto comp_is_rect = dynamic_cast<const rect *>(comp);
    if (comp_is_circle)
    {
      auto this_is_circle = dynamic_cast<const circle *>(this);
      auto this_is_rect = dynamic_cast<const rect *>(this);
      if (this_is_circle)
      {
        return circleCircleCollision(this_is_circle, comp_is_circle);
      }
      else if (this_is_rect)
      {
        return circleRectCollision(this_is_circle, this_is_rect);
      }
      else
      {
        assert(false && "Object in collision has no base type");
      }
    }
    else if (comp_is_rect)
    {
      auto this_is_circle = dynamic_cast<const circle *>(this);
      auto this_is_rect = dynamic_cast<const rect *>(this);
      if (this_is_circle)
      {
        return circleRectCollision(this_is_circle, comp_is_rect);
      }
      else if (this_is_rect)
      {
        return rectRectCollision(this_is_rect, comp_is_rect);
      }
      else
      {
        assert(false && "Object in collision has no base type");
      }
    }
    else
    {
      assert(false && "Object in collision has no base type");
    }
  }

}
