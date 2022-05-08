#pragma once
#include "math.hpp"
#include <vector>
#include <allegro5/allegro_primitives.h>

namespace mg8
{
    void drawSpline();

    typedef struct segment
    {
    public:
        vec2f a;
        vec2f b;
        vec2f c;
        vec2f d;
    } segment;

    typedef struct splineCurve
    {
    public:
        float alpha = 0.5f;
        float tension = 0.0f;
        std::vector<vec2f> controlPoints;
        std::vector<segment> segments;
        bool loop = true;

        void addControlPoint(vec2f point)
        {
            controlPoints.push_back(point);
        }

        int getVecIndex(int position)
        {
            if (position < 0)
            {
                return controlPoints.size() - 1;
            }
            else if (position > controlPoints.size())
            {
                return 1;
            }
            else if (position > controlPoints.size() - 1)
            {
                return 0;
            }
            else
            {
                return position;
            }
        }

        void drawCurve()
        {
            for (size_t i = 0; i < controlPoints.size(); i++)
            {
                drawSegments(i);
                al_draw_filled_circle(controlPoints[i].x, controlPoints[i].y, 2, al_map_rgba(255, 0, 0, 255));
            }
        }

        void drawSegments(int p1_index) // draw segments between point p1 and p2 -> points given as index to controlPoints vector
        {
            vec2f p0 = controlPoints[getVecIndex(p1_index - 1)];
            vec2f p1 = controlPoints[p1_index];
            vec2f p2 = controlPoints[getVecIndex(p1_index + 1)];
            vec2f p3 = controlPoints[getVecIndex(p1_index + 2)];

            vec2f curr_pos = p1;

            float curve_smoothness = 0.1f; // the smaller the smoother the interpolated curve gets
            // into how many segments we split the interpolation between p1 and p2
            int subdivisions = floorf(1.0f / curve_smoothness);

            for (int i = 1; i <= subdivisions; i++)
            {
                // subdivision position
                float t = i * curve_smoothness;

                // get subdivision point via Catmull-Rom spline
                vec2f newCurrPos = calcCatmullRom(t, p0, p1, p2, p3);

                // draw the segment
                al_draw_line(curr_pos.x, curr_pos.y, newCurrPos.x, newCurrPos.y, al_map_rgb(0, 0, 255), 2);

                // update position to draw from this point in the next iteration
                curr_pos = newCurrPos;
            }
        }

        vec2f calcCatmullRom(float t, vec2f p0, vec2f p1, vec2f p2, vec2f p3)
        {
            return (p1 * 2.0f + (p2 - p0) * t + (p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * t * t + ((p0 * -1) + p1 * 3.0f - p2 * 3.0f + p3) * t * t * t) * 0.5f;
        }
    } splineCurve;
}
