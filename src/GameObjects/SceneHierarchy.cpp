#include "GameObjects/SceneHierarchy.hpp"
#include <allegro5/allegro_primitives.h>
#include <spdlog/spdlog.h>
#include "GameCore/GameManager.hpp"

#include "stack"

namespace mg8
{
    SceneHierarchy::SceneHierarchy(GameObject *base)
    {
        s = new SceneGraph(base);
    }

    void SceneHierarchy::addSatellites(GameObject *parent, std::vector<GameObject *> children)
    {
        /*if (parent == s->root)
        {
            // first level
            s->children.push_back(children);
            return;
        }

        for (size_t i = 0; i < s->children.size(); i++)
        {
            if (s->children[i] == parent)
            {
                s->children[i].push_back(children);
            }
        }*/
    }

}