#include "GameObjects/Hole.hpp"
#include "GameObjects/RigidBody.hpp"
#include <allegro5/allegro_primitives.h>

#include "GameCore/GameManager.hpp"

#include "configuration.hpp"
#include "math/math.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

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
                if (obj->m_gameobject_type == TYPE_SATELLITE_BALL)
                {
                    return;
                }

                if (obj->m_gameobject_type == TYPE_WHITE_BALL)
                {
                    obj->m_velocity = {0, 0};
                    obj->circle::pos = {(float)config_start_resolution_w / 2.0f * 1.5f, (float)config_start_resolution_h / 2.0f};
                    return;
                }
                else if (obj->m_gameobject_type == TYPE_PLAYER1_BALL || obj->m_gameobject_type == TYPE_PLAYER2_BALL)
                {
                    auto &objects = GameManager::instance()->getGameObjects();
                    spdlog::info("erasing object.");
                    objects.erase(std::remove(objects.begin(), objects.end(), collider), objects.end());
                    GameManager::instance()->releaseGameObjects(true);
                }
                else if (obj->m_gameobject_type == TYPE_BLACK_BALL)
                {
                    /* code */
                }
                else
                {
                    assert(false && "Hole handle collision - Ball is not a white, black or play ball.");
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