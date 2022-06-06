#pragma once

#include "math/math.hpp"
#include "vector"
#include <allegro5/color.h>
#include "GameObject.hpp"

namespace mg8
{

    typedef struct SceneGraph
    {
        GameObject *parent;
        GameObject *obj;
        std::vector<SceneGraph *> children;

        SceneGraph(GameObject *parent, GameObject *obj) : parent(parent), obj(obj) {}
    } SceneGraph;

    class SceneHierarchy : public rect
    {

    private:
        SceneGraph *s;

    public:
        SceneHierarchy(GameObject *root);
        void addSatellites(GameObject *parent, std::vector<GameObject *> satellites);
        void updateVelocity(GameObject *obj);
    };
}