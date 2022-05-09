
#include "GameObjects/GameObject.hpp"
#include "math/math.hpp"
#include "configuration.hpp"
#include "GameObjects/RigidBody.hpp"

#include <spdlog/spdlog.h>
namespace mg8
{

  GameObject::GameObject(const MG8_OBJECT_TYPES type,
                         const uint32_t collision,
                         const vec2f velocity) : m_velocity(velocity),
                                                 m_type(type),
                                                 m_collision_mask(collision)

  {
  }

  GameObject::~GameObject()
  {
  }

  bool GameObject::is_moving() const
  {
    if (abs(m_velocity.mag()) > MOVEMENT_EPSILON)
    {
      return true;
    }
    return false;
  }

  bool GameObject::collides_with(const GameObject *comp) const
  {
    if (comp->m_type == TYPE_RIGID_BODY && this->m_type == TYPE_RIGID_BODY)
    {
      auto obj1 = dynamic_cast<const RigidBody *>(this);
      auto obj2 = dynamic_cast<const RigidBody *>(comp);

      if (obj2->m_rigid_body_type == TYPE_BALL)
      {
        auto comp_is_circle = dynamic_cast<const circle *>(comp);
        if (obj1->m_rigid_body_type == TYPE_BALL)
        {
          auto this_is_circle = dynamic_cast<const circle *>(this);
          return circleCircleCollision(this_is_circle, comp_is_circle);
        }
        else if (obj1->m_rigid_body_type == TYPE_RECTANGLE)
        {
          auto this_is_rect = dynamic_cast<const rect *>(this);
          return circleRectCollision(comp_is_circle, this_is_rect);
        }
        else
        {
          assert(false && "Object in collision has no base type");
        }
      }
      else if (obj2->m_rigid_body_type == TYPE_RECTANGLE)
      {
        auto comp_is_rect = dynamic_cast<const rect *>(comp);
        if (obj1->m_rigid_body_type == TYPE_BALL)
        {
          auto this_is_circle = dynamic_cast<const circle *>(this);
          return circleRectCollision(this_is_circle, comp_is_rect);
        }
        else if (obj1->m_rigid_body_type == TYPE_RECTANGLE)
        {
          return false; // rect - rect collision leads to constant collision, as the table borders touch all the time
          // auto this_is_rect = dynamic_cast<const rect *>(this);
          // return rectRectCollision(this_is_rect, comp_is_rect);
        }
        else
        {
          assert(false && "Object in collision has no base type");
        }
      }
    }

    /*auto comp_is_circle = dynamic_cast<const circle *>(comp);
    auto comp_is_rect = dynamic_cast<const rect *>(comp);

    spdlog::info("comp_is_circle: {}, comp_is_rect: {}", comp_is_circle ? true : false, comp_is_rect ? true : false);
    if (comp_is_circle)
    {
      auto this_is_circle = dynamic_cast<const circle *>(this);
      auto this_is_rect = dynamic_cast<const rect *>(this);
      spdlog::info("this_is_circle: {}, this_is_rect: {}", this_is_circle ? true : false, this_is_rect ? true : false);
      if (this_is_circle)
      {
        return circleCircleCollision(this_is_circle, comp_is_circle);
      }
      else if (this_is_rect)
      {
        spdlog::info("checking rect circ collision");
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
        spdlog::info("checking rect circ collision");
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
    }*/
  }
}
