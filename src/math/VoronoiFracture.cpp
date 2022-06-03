#include "math/VoronoiFracture.hpp"
#include <allegro5/allegro_primitives.h>
#include <spdlog/spdlog.h>
#include "GameCore/GameManager.hpp"

namespace mg8
{

    int mapValue(float value, float oldIntervalMin, float oldIntervalMax, int newIntervalMin, int newIntervalMax)
    {
        return newIntervalMin + ((newIntervalMax - newIntervalMin) / int(oldIntervalMax - oldIntervalMin)) * (int)(value - oldIntervalMin);
        // return (int)(value - oldIntervalMin) * (newIntervalMax - newIntervalMin) / (int)(oldIntervalMax - oldIntervalMin) + newIntervalMin;
    }

    VoronoiFracture::VoronoiFracture(rect *fractureBody) : fractureBody(fractureBody)
    {
        spdlog::info("VoronoiFracture called for (unrotated) rect with points lu: ({},{}), ru: ({},{}), ll: ({},{}), rl: ({},{})",
                     fractureBody->pos.x, fractureBody->pos.y, fractureBody->pos.x + fractureBody->width, fractureBody->pos.y,
                     fractureBody->pos.x, fractureBody->pos.y + fractureBody->height, fractureBody->pos.x + fractureBody->width, fractureBody->pos.y + fractureBody->height);
        int voronoiPoints = 15;
        for (size_t i = 0; i < voronoiPoints; i++)
        {
            vec2f point = fractureBody->get_random_point_in_unrotated_rect();
            spdlog::info("point[{}]: ({},{})", i, point.x, point.y);
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
            std::random_device r;
            std::mt19937 gen(r());
            std::uniform_int_distribution dis(0, 255);

            int red = dis(gen);
            int green = dis(gen);
            int blue = dis(gen);
            //  printf("prior cell[%d]: %f,%f\n", i, cells[i]->center.x, cells[i]->center.y);
            cells[i]->center = rotatePoint(cells[i]->center, fractureBody->get_anchor(), fractureBody->rotation);
            // printf("cell[%d]: %f,%f\n", i, cells[i]->center.x, cells[i]->center.y);
            // printf("cell[%d]: min_dist: %f, max_dist: %f\n", i, cells[i]->min_distance, cells[i]->max_distance);
            for (size_t j = 0; j < cells[i]->distanceField.size(); j++)
            {
                /*if (i == 0)
                {
                    printf("prior cell 0  point[%d]: %f,%f\n", j, cells[i]->distanceField[j]->point.x, cells[i]->distanceField[j]->point.y);
                }*/
                cells[i]->distanceField[j]->point = rotatePoint(cells[i]->distanceField[j]->point, fractureBody->get_anchor(), fractureBody->rotation);

                // TODO Set color

                /*int r_g_or_b = mapValue(cells[i]->distanceField[j]->distance, cells[i]->min_distance, cells[i]->max_distance, 0, 300);
                printf("r_g_or_b: %d\n", r_g_or_b);
                int red = 0;
                int green = 0;
                int blue = 0;
                if (r_g_or_b < 100)
                {
                    blue = mapValue(cells[i]->distanceField[j]->distance, cells[i]->min_distance, cells[i]->max_distance, 0, 255);
                    // printf("blue: %d\n", blue);
                }
                else if (r_g_or_b < 200)
                {
                    blue = 128;
                    green = mapValue(cells[i]->distanceField[j]->distance, cells[i]->min_distance, cells[i]->max_distance, 0, 255);
                }
                else
                {
                    blue = 128;
                    green = 128;
                    red = mapValue(cells[i]->distanceField[j]->distance, cells[i]->min_distance, cells[i]->max_distance, 0, 255);
                }*/

                cells[i]->distanceField[j]->color = al_map_rgb(red, green, blue);

                /*if (i == 0)
                {
                    printf("cell 0  point[%d]: %f,%f\n", j, cells[i]->distanceField[j]->point.x, cells[i]->distanceField[j]->point.y);
                }*/
            }
        }
    }

    void VoronoiFracture::draw()
    {
        for (size_t i = 0; i < cells.size(); i++)
        {
            for (size_t j = 0; j < cells[i]->distanceField.size(); j++)
            {

                // draw distance field point

                // al_draw_filled_circle(cells[i]->distanceField[j]->point.x, cells[i]->distanceField[j]->point.y, 0.5, al_map_rgb(red, green, blue));
                if (GameManager::instance()->debug_enabled)
                {
                    al_draw_filled_circle(cells[i]->distanceField[j]->point.x, cells[i]->distanceField[j]->point.y, 0.5, cells[i]->distanceField[j]->color);
                }
            }
            if (GameManager::instance()->debug_enabled)
            {
                al_draw_filled_circle(cells[i]->center.x, cells[i]->center.y, 2, al_map_rgb(0, 0, 0));
            }
        }
    }
}