#pragma once

#include "math/math.hpp"
#include "GameObjects/GameObject.hpp"
#include <atomic>
#include <allegro5/color.h>

namespace mg8
{
    class Hole : public virtual GameObject, public circle
    {
    public:
        ALLEGRO_COLOR m_color = {0, 0, 0, 255};

        Hole(
            MG8_OBJECT_TYPES type = MG8_OBJECT_TYPES::TYPE_TABLE_HOLE,
            vec2f position = {0, 0},
            vec2f velocity = {0, 0},
            int radius = 3,
            ALLEGRO_COLOR color = {0, 0, 0, 255},
            uint32_t collision = 0);

        ~Hole();

        virtual void draw() const;

        void check_collision(GameObject *collider);
    };

}
