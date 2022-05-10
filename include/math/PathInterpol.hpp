#pragma once
#include "math.hpp"
#include <vector>
#include <allegro5/allegro_primitives.h>
#include <spdlog/spdlog.h>

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

    typedef struct arcLen
    {
    public:
        int index;
        float t;
        float arc;

        arcLen(int index, float t, float arc) : index(index), t(t), arc(arc) {}
    } arcLen;

    typedef struct splineCurve
    {
    public:
        float alpha = 0.5f;
        float tension = 0.0f;
        std::vector<vec2f> controlPoints;
        std::vector<segment> segments;
        std::vector<arcLen> arcLenTable;
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
            if (controlPoints.empty())
            {
                return;
            }

            for (size_t i = 0; i < controlPoints.size(); i++)
            {
                drawSegments(i);
                drawArcPoints(i);
                al_draw_filled_circle(controlPoints[i].x, controlPoints[i].y, 2, al_map_rgba(255, 0, 0, 255));
            }
        }

        void calcCurve()
        {
            for (size_t i = 0; i < controlPoints.size(); i++)
            {
                calcSegments(i);
            }
        }

        void drawArcPoints(int p1_index) // draw points of arc length parametrization
        {
            vec2f p0 = controlPoints[getVecIndex(p1_index - 1)];
            vec2f p1 = controlPoints[p1_index];
            vec2f p2 = controlPoints[getVecIndex(p1_index + 1)];
            vec2f p3 = controlPoints[getVecIndex(p1_index + 2)];

            for (size_t i = 0; i < arcLenTable.size(); i++)
            {
                if (arcLenTable[i].index == p1_index)
                {
                    vec2f pos = calcCatmullRom(arcLenTable[i].t, p0, p1, p2, p3);
                    // draw the segment
                    al_draw_circle(pos.x, pos.y, 2, al_map_rgb(255, 255, 255), 1);
                }
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

        int smaller_than_index(float val, std::vector<arcLen> tmp)
        {
            for (size_t i = 0; i < tmp.size(); i++)
            {
                if (val <= tmp[i].arc)
                {
                    return i;
                }
            }
            return -1;
        }

        void calcSegments(int p1_index) // draw segments between point p1 and p2 -> points given as index to controlPoints vector
        {
            vec2f p0 = controlPoints[getVecIndex(p1_index - 1)];
            vec2f p1 = controlPoints[p1_index];
            vec2f p2 = controlPoints[getVecIndex(p1_index + 1)];
            vec2f p3 = controlPoints[getVecIndex(p1_index + 2)];

            vec2f curr_pos = p1;

            float curve_smoothness = 0.1f; // the smaller the smoother the interpolated curve gets
            // into how many segments we split the interpolation between p1 and p2
            int subdivisions = floorf(1.0f / curve_smoothness);
            float total_chord_length = 0.0f;
            std::vector<arcLen> tmp;

            for (int i = 1; i <= subdivisions; i++)
            {
                // subdivision position
                float t = i * curve_smoothness;

                // get subdivision point via Catmull-Rom spline
                vec2f newCurrPos = calcCatmullRom(t, p0, p1, p2, p3);

                // draw the segment
                // al_draw_line(curr_pos.x, curr_pos.y, newCurrPos.x, newCurrPos.y, al_map_rgb(0, 0, 255), 2);

                float chord_length = (newCurrPos - curr_pos).mag();
                total_chord_length += chord_length;
                tmp.push_back(arcLen(p1_index, t, total_chord_length));
                // update position to draw from this point in the next iteration
                curr_pos = newCurrPos;
            }

            float arcLength = total_chord_length / (float)tmp.size();
            std::vector<arcLen> tmp_new;
            // spdlog::info("arcLength: {}", arcLength);

            for (size_t i = 0; i < tmp.size(); i++)
            {
                float t = 0.0f;
                int j = smaller_than_index(arcLength, tmp);
                if (j == 0)
                {
                    float s = (arcLength - 0.0f) / (tmp[j].arc - 0.0f);
                    t = s * (tmp[j].t - 0) + 0;
                    // spdlog::info("s: {}, t: {}", s, t);
                }
                else
                {
                    float s = (arcLength - tmp[j - 1].arc) / (tmp[j].arc - tmp[j - 1].arc);
                    t = s * (tmp[j].t - tmp[j - 1].t) + tmp[j - 1].t;
                    // spdlog::info("s: {}, t: {}", s, t);
                }

                /*if (t == 0.0f)
{
    assert(false && "t == 0 - something went wrong");
}*/

                // tmp_new.push_back(arcLen(p1_index, t, arcLength));
                arcLenTable.push_back(arcLen(p1_index, t, arcLength));
                arcLength += total_chord_length / tmp.size();
            }
        }

        vec2f calcCatmullRom(float t, vec2f p0, vec2f p1, vec2f p2, vec2f p3)
        {
            return (p1 * 2.0f + (p2 - p0) * t + (p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * t * t + ((p0 * -1) + p1 * 3.0f - p2 * 3.0f + p3) * t * t * t) * 0.5f;
        }
    } splineCurve;
}
