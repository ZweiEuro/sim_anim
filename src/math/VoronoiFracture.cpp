#include "math/VoronoiFracture.hpp"
#include <allegro5/allegro_primitives.h>
#include <spdlog/spdlog.h>
#include "GameCore/GameManager.hpp"

#include "stack"

namespace mg8
{

    float mapValue(float value, float oldIntervalMin, float oldIntervalMax, float newIntervalMin, float newIntervalMax)
    {
        return (newIntervalMin + ((newIntervalMax - newIntervalMin) / (oldIntervalMax - oldIntervalMin)) * (value - oldIntervalMin));
        // return (int)(value - oldIntervalMin) * (newIntervalMax - newIntervalMin) / (int)(oldIntervalMax - oldIntervalMin) + newIntervalMin;
    }

    bool ccw(DistanceFieldPoint *a, DistanceFieldPoint *b, DistanceFieldPoint *c)
    {
        return (b->point.x - a->point.x) * (c->point.y - a->point.y) < (b->point.y - a->point.y) * (c->point.x - a->point.x);
    }

    void VoronoiFracture::calcConvexHull(int cell)
    {
        spdlog::info("calc convex hull for cell[{}]\n", cell);

        if (cells[cell]->distanceField.size() < 3)
        {
            spdlog::info("convex hull for cell[{}] failed -> less than 3 points in cell\n", cell);
            return;
        }

        std::sort(cells[cell]->distanceField.begin(), cells[cell]->distanceField.end(), [](DistanceFieldPoint *a, DistanceFieldPoint *b)
                  {if (a->point.x < b->point.x){return true;} return false; });

        std::vector<DistanceFieldPoint *> tmp;

        for (DistanceFieldPoint *point : cells[cell]->distanceField)
        {
            while (tmp.size() >= 2 && !ccw(tmp[tmp.size() - 2], tmp[tmp.size() - 1], point))
            {
                tmp.pop_back();
            }
            tmp.push_back(point);
        }

        int tmp_dp = tmp.size() + 1;
        for (auto rit = cells[cell]->distanceField.crbegin(); rit != cells[cell]->distanceField.crend(); ++rit)
        {
            auto dp = *rit;
            while (tmp.size() >= tmp_dp && !ccw(tmp[tmp.size() - 2], tmp[tmp.size() - 1], dp))
            {
                tmp.pop_back();
            }
            tmp.push_back(dp);
        }
        tmp.pop_back();

        for (DistanceFieldPoint *point : tmp)
        {
            cells[cell]->hull.push_back(point->point);
        }
    }

    VoronoiFracture::VoronoiFracture(rect *fractureBody) : fractureBody(fractureBody)
    {
        spdlog::info("VoronoiFracture called for (unrotated) rect with points lu: ({},{}), ru: ({},{}), ll: ({},{}), rl: ({},{})",
                     fractureBody->pos.x, fractureBody->pos.y, fractureBody->pos.x + fractureBody->width, fractureBody->pos.y,
                     fractureBody->pos.x, fractureBody->pos.y + fractureBody->height, fractureBody->pos.x + fractureBody->width, fractureBody->pos.y + fractureBody->height);
        int voronoiPoints = config_voronoi_cells;
        for (size_t i = 0; i < voronoiPoints; i++)
        {
            vec2f point = fractureBody->get_random_point_in_unrotated_rect();
            // spdlog::info("point[{}]: ({},{})", i, point.x, point.y);
            cells.push_back(new VoronoiCell(point));
        }

        //+ 0.5 to have the center of each pixel
        for (float x_pos = fractureBody->pos.x + 0.5f; x_pos < fractureBody->pos.x + fractureBody->width; x_pos++)
        {
            for (float y_pos = fractureBody->pos.y + 0.5f; y_pos < fractureBody->pos.y + fractureBody->height; y_pos++)
            {

                float min_distance = MAXFLOAT;
                size_t min_cell = 0;
                for (size_t i = 0; i < cells.size(); i++)
                {
                    // calc distance between point and cell centers
                    float distance = eucledianDistance(cells[i]->center, vec2f(x_pos, y_pos));
                    if (distance < min_distance)
                    {
                        min_distance = distance;
                        min_cell = i;
                    }
                }
                // add point to cell which it is closest to
                cells[min_cell]->distanceField.push_back(new DistanceFieldPoint(vec2f(x_pos, y_pos), min_distance));
                // printf("x_pos: %f, y_pos: %f\n", cells[min_cell]->distanceField.back()->point.x, cells[min_cell]->distanceField.back()->point.y);
                if (min_distance < cells[min_cell]->min_distance)
                {
                    cells[min_cell]->min_distance = min_distance;
                }
                if (min_distance > cells[min_cell]->max_distance)
                {
                    cells[min_cell]->max_distance = min_distance;
                }
            }
        }

        // rotate all points according to rotation of rectangle
        for (size_t i = 0; i < cells.size(); i++)
        {
            //  printf("prior cell[%d]: %f,%f\n", i, cells[i]->center.x, cells[i]->center.y);
            cells[i]->center = rotatePoint(cells[i]->center, fractureBody->get_anchor(), fractureBody->rotation);
            // printf("cell[%d]: %f,%f\n", i, cells[i]->center.x, cells[i]->center.y);
            // printf("cell[%d]: min_dist: %f, max_dist: %f\n", i, cells[i]->min_distance, cells[i]->max_distance);
            for (size_t j = 0; j < cells[i]->distanceField.size(); j++)
            {
                cells[i]->distanceField[j]->point = rotatePoint(cells[i]->distanceField[j]->point, fractureBody->get_anchor(), fractureBody->rotation);

                int red = 0;
                int colored_distance = (int)mapValue(cells[i]->distanceField[j]->distance, cells[i]->min_distance, cells[i]->max_distance, 0.f, 205.f);
                int green = 255 - colored_distance;
                int blue = 255 - colored_distance;

                cells[i]->distanceField[j]->color = al_map_rgb(red, green, blue);
            }
        }
    }
    void VoronoiFracture::fracture()
    {
        if (fractured)
        {
            return;
        }

        for (size_t i = 0; i < cells.size(); i++)
        {
            calcConvexHull(i);
        }
        fractured = true;
    }

    void VoronoiFracture::draw()
    {
        for (size_t i = 0; i < cells.size(); i++)
        {
            if (GameManager::instance()->debug_enabled)
            {
                for (size_t j = 0; j < cells[i]->distanceField.size(); j++)
                {

                    // draw distance field point

                    // al_draw_filled_circle(cells[i]->distanceField[j]->point.x, cells[i]->distanceField[j]->point.y, 0.5, al_map_rgb(red, green, blue));

                    al_draw_filled_circle(cells[i]->distanceField[j]->point.x, cells[i]->distanceField[j]->point.y, 0.8, cells[i]->distanceField[j]->color);
                }
            }
            if (GameManager::instance()->debug_enabled)
            {
                al_draw_filled_circle(cells[i]->center.x, cells[i]->center.y, 2, al_map_rgb(0, 0, 0));
            }
            if (cells[i]->hull.size())
            {
                for (size_t j = 0; j < cells[i]->hull.size() - 1; j++)
                {
                    al_draw_line(cells[i]->hull[j].x, cells[i]->hull[j].y, cells[i]->hull[j + 1].x, cells[i]->hull[j + 1].y, al_map_rgb(0, 0, 0), 1);
                }
            }

            /**/
        }
    }
}