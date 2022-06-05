#include "GameObjects/ParticleDynamics.hpp"
#include "configuration.hpp"
namespace mg8
{
  const float h = 0.0025;
  const float G = 6.674e-11;

  // acceleration according to euler

  vec2f GravityWell::velocity_to_well(const float mass_body, const vec2f &object_position, float dt) const
  {

    // F = G * m_1 * m_2 / r^2
    // F =  m*a   -> a = F/m  -> m/s = N / kg

    auto r = (object_position - pos).mag();

    auto F = (G * m_mass * mass_body) /
             (r * r);

    auto a = ((pos - object_position).dir()) * F / mass_body;

    if (r < rad) // catch ridiculous velocity values
    {
      return ((pos - object_position).dir()) * r;
    }

    return a * dt;
  }

  // We have acceleration at t0 and we want to know what the acceleration will be at t1
  vec2f GravityWell::euler(const GameObject &obj, float t1) const
  {

    auto body_circ = dynamic_cast<const circle *>(&obj);
    assert(body_circ && "Dynamic cast null for gravity calc");

    vec2f vel1 = obj.m_velocity; // what we want to calculate
    float t = h;
    // Iterating till the point at which we
    // need approximation
    while (t < t1)
    {
      vel1 += velocity_to_well(obj.m_mass, body_circ->pos, t) * h;
      t += h;
    }

    return vel1;
  }

  vec2f GravityWell::kutta(const GameObject &obj, float t1) const
  {

    auto body_circ = dynamic_cast<const circle *>(&obj);
    assert(body_circ && "Dynamic cast null for gravity calc");

    // Count number of iterations using step size or
    // step height h
    int n = (int)(t1 / h);

    vec2f k1, k2, k3, k4, k5;

    // Iterate for number of iterations
    vec2f vel1 = obj.m_velocity;
    float t = 0;
    for (int i = 1; i <= n; i++)
    {
      // Apply Runge Kutta Formulas to find
      // next value of y
      k1 = velocity_to_well(obj.m_mass, body_circ->pos, t) * h;
      k2 = velocity_to_well(obj.m_mass, body_circ->pos + k1 * 0.5f, t + 0.5 * h) * h;
      k3 = velocity_to_well(obj.m_mass, body_circ->pos + k2 * 0.5f, t + 0.5 * h) * h;
      k4 = velocity_to_well(obj.m_mass, body_circ->pos + k3, t + h) * h;

      // Update next value of y
      vel1 = vel1 + (k1 + k2 * 2 + k3 * 3 + k4) * (1.0 / 6.0);

      // Update next value of x
      t += h;
    }

    return vel1;
  }

  void GravityWell::apply(std::vector<GameObject *> &objs, float dt) const
  {
    vec2f new_vel;

    for (const auto &obj : objs)
    {
      if (obj == this)
        continue;

      if (false)
      {
        new_vel = euler(*obj, dt);
      }
      else
      {
        new_vel = kutta(*obj, dt);
      }

      assert(!std::isnan(new_vel.x) && !std::isnan(new_vel.y) && "One or more velocities acquired nan");

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