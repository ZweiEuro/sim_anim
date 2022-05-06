#include "GameObjects/Hole.hpp"
#include "GameObjects/RigidBody.hpp"
#include <allegro5/allegro_primitives.h>

#include "GameCore/GameManager.hpp"

#include "configuration.hpp"
#include "math/math.hpp"

namespace mg8
{
    void Hole::draw() const
    {
        al_draw_filled_circle(pos.x, pos.y, rad, m_color);
        // al_draw_filled_circle((float)config_start_resolution_w / 2.0f, (float)config_start_resolution_h / 2.0f, 10, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
    }

    void Hole::move(vec2f delta_move)
    {
        return;
    }

    void Hole::handle_collision(GameObject *collider)
    {
        if (collider->m_type == TYPE_RIGID_BODY)
        {
            auto obj = dynamic_cast<RigidBody *>(collider);
            if (obj->m_rigid_body_type == TYPE_BALL)
            {
                // check if the ball is on the edge of the hole or falls into it
                bool in_hole = powf(obj->circle::pos.x - this->circle::pos.x, 2) + powf(obj->circle::pos.y - this->circle::pos.y, 2) < powf(this->circle::rad, 2);
                if (in_hole)
                {
                    auto objects = GameManager::instance()->getGameObjects();
                    std::vector<GameObject *>::iterator object_to_delete = std::find(objects.begin(), objects.end(), collider);
                    if (object_to_delete != objects.end())
                    {
                        delete *object_to_delete;
                        objects.erase(object_to_delete);
                    }

                    GameManager::instance()->releaseGameObjects(true);
                }
            }
        }
    }

    Hole::Hole(MG8_OBJECT_TYPES type,
               vec2f position,
               vec2f velocity,
               float radius,
               ALLEGRO_COLOR color,
               uint32_t collision) : GameObject(type, collision, velocity),
                                     circle(position, radius),
                                     m_color(color)
    {
    }
}