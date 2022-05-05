#include "GameObjects/RigidBody.hpp"
#include <allegro5/allegro_primitives.h>

#include "configuration.hpp"
#include "math/math.hpp"
#include <algorithm>

namespace mg8
{
  void RigidBody::draw() const
  {
    if (this->m_rigid_body_type == TYPE_BILIARD_BALL)
    {
      al_draw_filled_circle(circle::pos.x, circle::pos.y, rad, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
    }
    if (this->m_rigid_body_type == TYPE_TABLE_BORDER)
    {
      al_draw_filled_rectangle(rect::pos.x, rect::pos.y, rect::pos.x + rect::width, rect::pos.y + rect::height, al_map_rgb(102, 51, 0));
    }
  }
  void RigidBody::move(vec2f delta_move)
  {
    if (this->m_rigid_body_type == TYPE_BILIARD_BALL)
    {
      circle::setPosition(circle::pos + delta_move);
    }
    if (this->m_rigid_body_type == TYPE_TABLE_BORDER)
    {
      return;
    }
  }

  void RigidBody::handle_collision(RigidBody *collisioner)
  {
    if (this->m_rigid_body_type == TYPE_BILIARD_BALL && collisioner->m_rigid_body_type == TYPE_BILIARD_BALL)
    {
      handle_ball_ball_collision(collisioner);
    }
    else if (this->m_rigid_body_type == TYPE_BILIARD_BALL && collisioner->m_rigid_body_type == TYPE_TABLE_BORDER)
    {
      handle_ball_border_collision(this, collisioner);
    }
    else if (this->m_rigid_body_type == TYPE_TABLE_BORDER && collisioner->m_rigid_body_type == TYPE_BILIARD_BALL)
    {
      handle_ball_border_collision(collisioner, this);
    }
    else
    {
      // assert(false && "Border - border collision should not happen.");
    }
  }

  void RigidBody::handle_ball_ball_collision(RigidBody *otherBall)
  {
    vec2f pos_delta = this->circle::pos - otherBall->circle::pos;
    float r = this->rad + otherBall->rad;

    float d = pos_delta.mag();

    if (d == 0.0f)
    {
      // special case - balls are exactly on top of each other
      d = r - 1.0f;
      pos_delta = vec2f(r, 0.0f);
    }
    vec2f min_translational_dist = pos_delta * ((r - d) / d);

    float inverseMass_self = 1 / this->m_mass;
    float inverseMass_other = 1 / otherBall->m_mass;

    this->circle::pos = this->circle::pos + min_translational_dist * (inverseMass_self / (inverseMass_self + inverseMass_other));
    otherBall->circle::pos = otherBall->circle::pos - min_translational_dist * (inverseMass_other / (inverseMass_self + inverseMass_other));

    vec2f velocity = this->m_velocity - otherBall->m_velocity;
    float vn = velocity.dot(min_translational_dist.dir());
    if (vn > 0.0f)
    {
      // balls are already moving apart
      return;
    }
    float i = (-(1.0f + this->m_restitution_coeff) * vn) / (inverseMass_self + inverseMass_other);
    vec2f impulse = min_translational_dist * i;

    this->m_velocity = this->m_velocity + impulse * inverseMass_self;
    otherBall->m_velocity = otherBall->m_velocity - impulse * inverseMass_other;
  }

  void RigidBody::handle_ball_border_collision(RigidBody *ball, RigidBody *border)
  {
    float x_near = std::clamp(ball->circle::pos.x, border->rect::pos.x, border->rect::pos.x + border->rect::width);
    float y_near = std::clamp(ball->circle::pos.y, border->rect::pos.y, border->rect::pos.y + border->rect::height);

    vec2f ball_border_dist = vec2f(ball->circle::pos.x - x_near, ball->circle::pos.y - y_near);

    if (ball->m_velocity.dot(ball_border_dist) < 0)
    { // ball is moving towards border
      vec2f tangent_velocity = ball_border_dist.dir().dot(ball->m_velocity);
      ball->m_velocity = ball->m_velocity - tangent_velocity * 2;
    }

    float border_penetration = ball->circle::rad - ball_border_dist.mag();
    vec2f border_penetration_vector = ball_border_dist.dir() * border_penetration;
    ball->circle::pos = ball->circle::pos - border_penetration_vector;
  }

  // BilliardBall constructor
  RigidBody::RigidBody(MG8_RIGID_BODY_OBJECT_TYPES rigid_body_type,
                       vec2f position,
                       vec2f velocity,
                       int radius,
                       vec2f acceleration,
                       float mass,
                       float restitution_coeff,
                       ALLEGRO_COLOR color,
                       uint32_t collision,
                       MG8_OBJECT_TYPES obj_type) : GameObject(obj_type, collision, velocity),
                                                    circle(position, radius),
                                                    m_color(color), m_acceleration(acceleration), m_mass(mass), m_restitution_coeff(restitution_coeff), m_rigid_body_type(rigid_body_type)
  {
  }
  // TableBorder constructor
  RigidBody::RigidBody(MG8_RIGID_BODY_OBJECT_TYPES rigid_body_type,
                       vec2f position,
                       vec2f velocity,
                       float width,
                       float height,
                       vec2f acceleration,
                       float mass,
                       float restitution_coeff,
                       ALLEGRO_COLOR color,
                       uint32_t collision,
                       MG8_OBJECT_TYPES obj_type) : GameObject(obj_type, collision, velocity),
                                                    rect(position, width, height),
                                                    m_color(color), m_acceleration(acceleration), m_mass(mass), m_restitution_coeff(restitution_coeff), m_rigid_body_type(rigid_body_type)
  {
  }
}