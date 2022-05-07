#include "math/math.hpp"
#include <algorithm>
namespace mg8
{

  bool rect::point_inside(vec2f point) const
  {
    return (pos.x <= point.x && point.x <= pos.x + width) && (pos.y >= point.y && point.y >= pos.y - height);
  }

  vec2f sub(vec2f a, vec2f b)
  {
    a.x -= b.x;
    a.y -= b.y;
    return a;
  }
  // float dot(vec2f a, vec2f b) { return a.x * b.x + a.y * b.y; }

  bool circleCircleCollision(const circle *A, const circle *B)
  {
    assert(A && B && "Circle circle collision with nullptr");
    vec2f c = sub(B->pos, A->pos);
    // float d2 = dot(c, c);
    float d2 = c.dot(c);
    float r2 = A->rad + B->rad;
    r2 = r2 * r2;
    return d2 < r2;
  }

  bool circleRectCollision(const circle *A, const rect *B)
  {

    float x_near = std::clamp(A->pos.x, B->pos.x, B->pos.x + B->width);
    float y_near = std::clamp(A->pos.y, B->pos.y, B->pos.y + B->height);

    vec2f circle_rect_dist = vec2f(A->pos.x - x_near, A->pos.y - y_near);

    // If dist < circle radius -> intersection = true
    float dist_squared = (circle_rect_dist.x * circle_rect_dist.x) + (circle_rect_dist.y * circle_rect_dist.y);
    return dist_squared < (A->rad * A->rad);

    /*float dist_x = fabsf(A->pos.x - (B->pos.x - B->width / 2));
    float dist_y = fabsf(A->pos.y - (B->pos.y - B->height / 2));

    if (dist_x > (B->width / 2 + A->rad))
    {
      return false;
    }
    if (dist_y > (B->height / 2 + A->rad))
    {
      return false;
    }

    if (dist_x <= (B->width / 2))
    {
      return true;
    }
    if (dist_y <= (B->height / 2))
    {
      return true;
    }

    float dx = dist_x - B->width / 2;
    float dy = dist_y - B->height / 2;
    return (dx * dx + dy * dy <= (A->rad * A->rad));*/
    // assert(A && B && "Circle poly collision with nullptr");
    // return false;
    //  https://stackoverflow.com/a/402019/12258809

    // use point in circle and point in  rect to realize
    // point in circle is easy with its radius. point distance to circle center, calculate length and if smaller than radius its inside if not its outside
  }

  bool rectRectCollision(const rect *A, const rect *B)
  {
    assert(A && B && "poly poly collision with nullptr");

    // very trivial approach: Check for every point in A if its between the Y and X coordinates of B

    vec2f a = B->pos, b = B->pos, c = B->pos, d = B->pos;

    b.x += B->width;  // top right
    c.y -= B->height; // bottom left

    // bottom right
    d.x += B->width;
    d.y -= B->height;

    return A->point_inside(a) ||
           A->point_inside(b) ||
           A->point_inside(c) ||
           A->point_inside(d);
  }

  vec2f rotatePoint(const vec2f point, const vec2f anchor_point, const float angle)
  {
    float rotation_angle = angle * M_PI / 180.0f;
    float rotated_point_x = cosf(rotation_angle) * (point.x - anchor_point.x) - sinf(rotation_angle) * (point.y - anchor_point.y) + anchor_point.x;
    float rotated_point_y = sinf(rotation_angle) * (point.x - anchor_point.x) + cosf(rotation_angle) * (point.y - anchor_point.y) + anchor_point.y;
    return vec2f(rotated_point_x, rotated_point_y);
  }
}