
#include "GameObjects/GameObject.hpp"
#include "math/math.hpp"
#include "configuration.hpp"
#include "GameObjects/RigidBody.hpp"
#include "GameObjects/Hole.hpp"

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

  bool GameObject::collides_with(const RigidBody *comp) const
  {

    auto me_rigid = dynamic_cast<const RigidBody *>(this);

    if (me_rigid)
    {
      if (comp->m_rigid_body_type == TYPE_BALL && me_rigid->m_rigid_body_type == TYPE_BALL)
      {
        return circleCircleCollision(me_rigid, comp);
      }
      else if (comp->m_rigid_body_type == TYPE_BALL && me_rigid->m_rigid_body_type == TYPE_RECTANGLE)
      {
        return circleRectCollision(comp, me_rigid);
      }
      else if (comp->m_rigid_body_type == TYPE_RECTANGLE && me_rigid->m_rigid_body_type == TYPE_BALL)
      {
        return circleRectCollision(me_rigid, comp);
      }
      else if (comp->m_rigid_body_type == TYPE_RECTANGLE && me_rigid->m_rigid_body_type == TYPE_RECTANGLE)
      {
        return false;
      }
      else
      {
        assert(false && "Object in collision has no base type");
      }
    }
    return false;
  }

  bool GameObject::collides_with(const Hole *comp) const
  {

    auto me_rigid = dynamic_cast<const RigidBody *>(this);

    if (me_rigid)
    {
      if (me_rigid->m_rigid_body_type == TYPE_BALL)
      {
        return circleHoleCollision(me_rigid, comp);
      }
      else if (me_rigid->m_rigid_body_type == TYPE_RECTANGLE)
      {
        // no rectangle - hole collision
        return false;
      }
      else
      {
        assert(false && "Object in collision has no base type");
      }
    }
    return false;
  }
}
