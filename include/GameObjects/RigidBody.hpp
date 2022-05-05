#pragma once

#include "math/math.hpp"
#include "GameObjects/GameObject.hpp"
#include <atomic>
#include <allegro5/color.h>

namespace mg8
{

    enum MG8_RIGID_BODY_OBJECT_TYPES : uint32_t
    {
        TYPE_UNDEFINED_BODY = 0,
        TYPE_BILIARD_BALL = 1 << 0,
        TYPE_TABLE_BORDER = 1 << 1,
        // TYPE_RIGHT_TABLE_BORDER = 1 << 1,
        // TYPE_LEFT_TABLE_BORDER = 1 << 2,
        // TYPE_UPPER_TABLE_BORDER = 1 << 3,
        // TYPE_LOWER_TABLE_BORDER = 1 << 4,
    };

    class RigidBody : public virtual GameObject, public circle, public rect
    {
    public:
        ALLEGRO_COLOR m_color = {0, 0, 0, 255};

        vec2f m_acceleration = {0, 0};
        float m_mass = 1.0f;
        float m_restitution_coeff = 0;

        MG8_RIGID_BODY_OBJECT_TYPES m_rigid_body_type = TYPE_UNDEFINED_BODY;

        // bool collides_with(const GameObject *comp) const;
        // virtual void handle_collision(GameObject *collision_with) = 0;

        // BilliardBall constructor
        RigidBody(MG8_RIGID_BODY_OBJECT_TYPES rigid_body_type = TYPE_BILIARD_BALL,
                  vec2f position = {0, 0},
                  vec2f velocity = {0, 0},
                  int radius = 1,
                  vec2f acceleration = {0, 0},
                  float mass = 0.2f,
                  float restitution_coeff = 0.93,
                  ALLEGRO_COLOR color = {0, 0, 0, 255},
                  uint32_t collision = 0,
                  MG8_OBJECT_TYPES obj_type = TYPE_RIGID_BODY);

        // TableBorder constructor
        RigidBody(MG8_RIGID_BODY_OBJECT_TYPES rigid_body_type = TYPE_TABLE_BORDER,
                  vec2f position = {0, 0},
                  vec2f velocity = {0, 0},
                  float width = 1.0f,
                  float height = 1.0f,
                  vec2f acceleration = {0, 0},
                  float mass = 1.0f,
                  float restitution_coeff = 0.8,
                  ALLEGRO_COLOR color = {102, 51, 0, 255},
                  uint32_t collision = 0,
                  MG8_OBJECT_TYPES obj_type = TYPE_RIGID_BODY);

        ~RigidBody();

        virtual void draw() const;
        virtual void move(vec2f delta_move);
        void handle_collision(RigidBody *collider);
        void handle_ball_ball_collision(RigidBody *collider);
        void handle_ball_border_collision(RigidBody *ball, RigidBody *border);
    };

}
