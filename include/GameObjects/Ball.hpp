#pragma once
#include "GameObjects/GameObject.hpp"
#include <allegro5/color.h>
namespace mg8
{

  class Ball : public virtual GameObject, public circle
  {
  public:
    ALLEGRO_COLOR m_color = {0, 0, 0, 255};
    vec2f m_acceleration = {0, 0};
    float m_mass = 1.0f;

    Ball(
        MG8_OBJECT_TYPES type,
        vec2f position = {0, 0},
        vec2f velocity = {0, 0},
        int radius = 1,
        vec2f acceleration = {0, 0},
        float mass = 1.0f,
        ALLEGRO_COLOR color = {0, 0, 0, 255},
        uint32_t collision = 0);

    virtual void draw() const;
    virtual void move(vec2f delta_move);
    void handle_collision(Ball *otherBall);

    bool collides_with(const GameObject *comp) const;
  };
}