#include "GameObjects/ParticleDynamics.hpp"

namespace mg8
{
  const float h = 0.025;
  const float G = 6.674e-11;

  // acceleration according to euler

  vec2f GravityWell::velocity_to_well(const GameObject &body, float dt)
  {

    // F = G * m_1 * m_2 / r^2
    // F =  m*a   -> a = F/m  -> m/s = N / kg

    auto body_circ = dynamic_cast<const circle *>(&body);
    assert(body_circ && "Dynamic cast null for gravity calc");

    auto F = (G * m_mass * body.m_mass) /
             ((body_circ->pos - pos).mag() * (body_circ->pos - pos).mag());

    auto a = ((body_circ->pos - pos).dir()) * F * body.m_mass;

    return a * dt;
  }

  // We have acceleration at t0 and we want to know what the acceleration will be at t1
  vec2f GravityWell::euler(const GameObject &obj, float t1)
  {

    vec2f vel1 = obj.m_velocity; // what we want to calculate
    float t = 0;
    // Iterating till the point at which we
    // need approximation
    while (t < t1)
    {
      vel1 += velocity_to_well(obj, t) * h;
      t += h;
    }

    return vel1;
  }

  void GravityWell::apply(const std::vector<GameObject> &objs, float dt)
  {

    for (const auto &obj : objs)
      if (true)
      {
        euler(obj, dt);
      }
      else
      {
        abort();
      }
  }

  GravityWell::GravityWell(MG8_OBJECT_TYPES type,
                           vec2f position,
                           vec2f velocity,
                           float radius,
                           ALLEGRO_COLOR color,
                           uint32_t collision) : GameObject(type, collision, velocity),
                                                 circle(position, radius),
                                                 m_color(color)
  {
  }

  void GravityWell::draw() const
  {
    al_draw_filled_circle(pos.x, pos.y, rad, m_color);
    // al_draw_filled_circle((float)config_start_resolution_w / 2.0f, (float)config_start_resolution_h / 2.0f, 10, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
  }

  void GravityWell::move(vec2f delta_move)
  {
    return;
  }
}