#include "GameObjects/ParticleDynamics.hpp"
#include "configuration.hpp"
namespace mg8
{
  const float h = 0.0025;
  const float G = 6.674e-11;

  // acceleration according to euler

  vec2f GravityWell::velocity_to_well(const GameObject &body, float dt) const
  {

    // F = G * m_1 * m_2 / r^2
    // F =  m*a   -> a = F/m  -> m/s = N / kg

    auto body_circ = dynamic_cast<const circle *>(&body);
    assert(body_circ && "Dynamic cast null for gravity calc");

    auto r = (body_circ->pos - pos).mag();

    if (r < config_min_distance_grav_well)
    {
      return 0.0f;
    }
    auto F = (G * m_mass * body.m_mass) /
             (r * r);

    auto a = ((pos - body_circ->pos).dir()) * F * body.m_mass;

    return a * dt;
  }

  // We have acceleration at t0 and we want to know what the acceleration will be at t1
  vec2f GravityWell::euler(const GameObject &obj, float t1) const
  {

    vec2f vel1 = obj.m_velocity; // what we want to calculate
    float t = h;
    // Iterating till the point at which we
    // need approximation
    while (t < t1)
    {
      vel1 += velocity_to_well(obj, t) * h;
      t += h;
    }

    return vel1;
  }

  void GravityWell::apply(std::vector<GameObject *> &objs, float dt) const
  {
    vec2f new_vel;
    for (const auto &obj : objs)
    {
      if (true)
      {
        new_vel = euler(*obj, dt);
      }
      else
      {
        abort();
      }

      if (new_vel.mag() > MAX_VEL.mag())
      {
        new_vel = {0,
                   0};
      }

      obj->m_velocity = new_vel;
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

    al_draw_circle(pos.x, pos.y, rad, m_color, 1.0f);
    // al_draw_filled_circle((float)config_start_resolution_w / 2.0f, (float)config_start_resolution_h / 2.0f, 10, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
  }

  void GravityWell::move(vec2f delta_move)
  {
    return;
  }
}