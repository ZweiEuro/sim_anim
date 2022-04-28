#include "GameObjects/Ball.hpp"
#include <allegro5/allegro_primitives.h>

#include "configuration.hpp"

namespace mg8
{
  void Ball::draw() const
  {
    al_draw_filled_circle(pos.x, pos.y, rad, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
    // al_draw_filled_circle((float)config_start_resolution_w / 2.0f, (float)config_start_resolution_h / 2.0f, 10, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
  }

  Ball::Ball(MG8_OBJECT_TYPES type,
             vec2 position,
             vec2 velocity,
             int radius,
             ALLEGRO_COLOR color,
             uint32_t collision) : GameObject(type, collision, velocity),
                                   circle(position, radius),
                                   m_color(color)
  {
  }

}