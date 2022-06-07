#include "GameObjects/SceneHierarchy.hpp"
#include <allegro5/allegro_primitives.h>
#include <spdlog/spdlog.h>
#include "GameCore/GameManager.hpp"
#include "GameObjects/RigidBody.hpp"

#include "stack"

namespace mg8
{
    SceneHierarchy::SceneHierarchy(GameObject *base)
    {
        s = new SceneGraph(nullptr, base); // white ball -> parent == null, obj == white ball
    }

    void SceneHierarchy::addSatellites(GameObject *parent, std::vector<GameObject *> children)
    {
        if (parent == s->obj) // if root
        {
            for (size_t i = 0; i < children.size(); i++) // add L1 nodes -> parent == white ball
            {
                s->children.push_back(new SceneGraph(parent, children[i]));
            }
            return;
        }

        for (size_t i = 0; i < s->children.size(); i++)
        {
            if (s->children[i]->obj == parent) // add L2 nodes -> parent == L1 node
            {
                s->children[i]->children.push_back(new SceneGraph(parent, children[i]));
                return;
            }
            for (size_t j = 0; j < s->children[i]->children.size(); j++)
            {
                if (s->children[i]->children[j]->obj == parent) // add L3 nodes -> parent == L2 node
                {
                    s->children[i]->children[j]->children.push_back(new SceneGraph(parent, children[i]));
                    return;
                }
            }
        }
    }

    void SceneHierarchy::updateVelocity(GameObject *obj)
    {
        for (size_t i = 0; i < s->children.size(); i++)
        {
            if (obj == s->children[i]->obj)
            {
                // spdlog::info("set l1 velocity to parent velocity: {},{}", s->children[i]->parent->m_velocity.x, s->children[i]->parent->m_velocity.y);

                // vector from parent center to obj center -> vector orthogonal to this -> vector of obj rotating around parent

                // obj->center - parent-> center //vector from parent to obj
                RigidBody *p = dynamic_cast<RigidBody *>(s->children[i]->parent);
                RigidBody *c = dynamic_cast<RigidBody *>(s->children[i]->obj);

                vec2f v = c->circle::pos - p->circle::pos; // vector from p to c
                vec2f v_ = v * (55.0f) / v.mag();
                vec2f pos_fix = v - v_;
                c->circle::pos = c->circle::pos - (pos_fix);
                vec2f v_rotated = vec2f(-v_.y, v_.x);
                vec2f c_vel = v_rotated * 2.5f;

                s->children[i]->obj->m_velocity = c_vel + s->children[i]->parent->m_velocity; // set velocities L1
                return;
            }
            for (size_t j = 0; j < s->children[i]->children.size(); j++)
            {
                if (obj == s->children[i]->children[j]->obj)
                {
                    // obj->center - parent-> center //vector from parent to obj
                    RigidBody *p = dynamic_cast<RigidBody *>(s->children[i]->children[j]->parent);
                    RigidBody *c = dynamic_cast<RigidBody *>(s->children[i]->children[j]->obj);

                    vec2f v = c->circle::pos - p->circle::pos; // vector from p to c
                    vec2f v_ = v * (25.0f) / v.mag();
                    vec2f pos_fix = v - v_;
                    c->circle::pos = c->circle::pos - (pos_fix);
                    vec2f v_rotated = vec2f(v_.y, -v_.x);
                    vec2f c_vel = v_rotated * 4.5f;
                    s->children[i]->children[j]->obj->m_velocity = c_vel + s->children[i]->children[j]->parent->m_velocity; // set velocities L2
                    return;
                }

                for (size_t k = 0; k < s->children[i]->children[j]->children.size(); k++)
                {
                    if (obj == s->children[i]->children[j]->children[k]->obj)
                    {
                        // obj->center - parent-> center //vector from parent to obj
                        RigidBody *p = dynamic_cast<RigidBody *>(s->children[i]->children[j]->children[k]->parent);
                        RigidBody *c = dynamic_cast<RigidBody *>(s->children[i]->children[j]->children[k]->obj);

                        vec2f v = c->circle::pos - p->circle::pos; // vector from p to c
                        vec2f v_ = v * (15.0f) / v.mag();
                        vec2f pos_fix = v - v_;
                        c->circle::pos = c->circle::pos - (pos_fix);
                        vec2f v_rotated = vec2f(-v_.y, v_.x);
                        vec2f c_vel = v_rotated * 5.5f;
                        s->children[i]->children[j]->children[k]->obj->m_velocity = c_vel + s->children[i]->children[j]->children[k]->parent->m_velocity; // set velocities L3
                        return;
                    }
                }
            }
        }
    }

}