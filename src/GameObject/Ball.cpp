#include "GameObject/Ball.hpp"
#include <allegro5/allegro_primitives.h>

#include "configuration.hpp"

namespace mg8
{
  void Ball::draw() const
  {
    al_draw_filled_circle(m_position.x, m_position.y, m_radius, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
    // al_draw_filled_circle((float)config_start_resolution_w / 2.0f, (float)config_start_resolution_h / 2.0f, 10, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
  }

  Ball::Ball(vec2 position,
             vec2 velocity,
             int radius,
             ALLEGRO_COLOR color,
             uint32_t collision) : GameObject(MG8_OBJECT_TYPES::BALL, collision, position, velocity),
                                   m_radius(radius),
                                   m_color(color)
  {
  }

}