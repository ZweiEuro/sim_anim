#include "GameObjects/Ball.hpp"
#include <allegro5/allegro_primitives.h>

#include "configuration.hpp"
#include "math/math.hpp"

namespace mg8
{
  void Ball::draw() const
  {
    al_draw_filled_circle(pos.x, pos.y, rad, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
    // al_draw_filled_circle((float)config_start_resolution_w / 2.0f, (float)config_start_resolution_h / 2.0f, 10, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
  }
  void Ball::move(vec2f delta_move)
  {
    circle::setPosition(pos + delta_move);
  }

  void Ball::handle_collision(Ball *otherBall)
  {
    vec2f pos_delta = this->pos - otherBall->pos;
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

    this->pos = this->pos + min_translational_dist * (inverseMass_self / (inverseMass_self + inverseMass_other));
    otherBall->pos = otherBall->pos - min_translational_dist * (inverseMass_other / (inverseMass_self + inverseMass_other));

    vec2f velocity = this->m_velocity - otherBall->m_velocity;
    float vn = velocity.dot(min_translational_dist.dir());
    if (vn > 0.0f)
    {
      // balls are already moving apart
      return;
    }
    float restitution = 0.9f;
    float i = (-(1.0f + restitution) * vn) / (inverseMass_self + inverseMass_other);
    vec2f impulse = min_translational_dist * i;

    this->m_velocity = this->m_velocity + impulse * inverseMass_self;
    otherBall->m_velocity = otherBall->m_velocity - impulse * inverseMass_other;
  }

  Ball::Ball(MG8_OBJECT_TYPES type,
             vec2f position,
             vec2f velocity,
             int radius,
             vec2f acceleration,
             float mass,
             ALLEGRO_COLOR color,
             uint32_t collision) : GameObject(type, collision, velocity),
                                   circle(position, radius),
                                   m_color(color), m_acceleration(acceleration), m_mass(mass)
  {
  }
}