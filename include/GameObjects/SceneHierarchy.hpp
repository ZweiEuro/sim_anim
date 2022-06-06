#pragma once

#include "math/math.hpp"
#include "vector"
#include <allegro5/color.h>
#include "GameObject.hpp"

namespace mg8
{

    typedef struct Layer
    {
        GameObject *parent;
        std::vector<GameObject *> children;
    } Layer;

    typedef struct SceneGraph
    {
        GameObject *root;
        std::vector<Layer *> layer;

        SceneGraph(GameObject *root) : root(root) {}
    } SceneGraph;

    class SceneHierarchy : public rect
    {

    private:
        SceneGraph *s;

    public:
        SceneHierarchy(GameObject *root);
        void addSatellites(GameObject *parent, std::vector<GameObject *> satellites);
        void updateVelocities();
    };
}