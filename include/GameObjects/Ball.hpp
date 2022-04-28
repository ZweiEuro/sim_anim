#pragma once
#include "GameObjects/GameObject.hpp"
#include <allegro5/color.h>
namespace mg8
{

  class Ball : public virtual GameObject, public circle
  {
  public:
    ALLEGRO_COLOR m_color = {0, 0, 0, 255};

    Ball(
        MG8_OBJECT_TYPES type,
        vec2 position = {0, 0},
        vec2 velocity = {0, 0},
        int radius = 1,
        ALLEGRO_COLOR color = {0, 0, 0, 255},
        uint32_t collision = 0);

    virtual void draw() const;
    virtual void move(vec2 delta_move);

    bool collides_with(const GameObject *comp) const;
  };
}