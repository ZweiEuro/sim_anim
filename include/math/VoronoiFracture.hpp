#pragma once

#include "math.hpp"
#include "vector"
#include <allegro5/color.h>

namespace mg8
{

    typedef struct DistanceFieldPoint
    {
        vec2f point;
        float distance;
        ALLEGRO_COLOR color;

        DistanceFieldPoint(vec2f point, float distance) : point(point), distance(distance) {}

    } DistanceFieldPoint;

    typedef struct VoronoiCell
    {
        vec2f center;
        float min_distance;
        float max_distance;
        std::vector<vec2f> hull;
        std::vector<DistanceFieldPoint *> distanceField;

        VoronoiCell(vec2f center, float min_distance = MAXFLOAT, float max_distance = -1000.0f) : center(center), min_distance(min_distance), max_distance(max_distance) {}
    } VoronoiCell;

    class VoronoiFracture : public rect
    {

    private:
        std::vector<VoronoiCell *> cells;
        rect *fractureBody;

        bool fractured = false;
        bool noised = false;

        void calcConvexHull(int cell_index);

    public:
        VoronoiFracture(rect *fractureBody);
        void draw();
        void fracture();
        void recalc();
        void toggleNoised()
        {
            if (noised)
            {
                noised = false;
            }
            else
            {
                noised = true;
            }
        }
    };
}