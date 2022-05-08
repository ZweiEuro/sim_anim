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

  float getTriangleArea(vec2f A, vec2f B, vec2f C)
  {
    vec2f AB = B - A;
    vec2f AC = C - A;
    float cross = AB.x * AC.y - AB.y * AC.x;
    return fabsf(cross) / 2.0f;
  }

  bool circleLineCollision(const circle *A, vec2f P, vec2f Q) // intersection of circle with line from P to Q
  {
    float triangleArea = getTriangleArea(A->rad, P, Q);
    float distance_PQ = sqrtf(powf(Q.x - P.x, 2) + powf(Q.y - P.y, 2));
    float triangleHeight = (2 * triangleArea) / distance_PQ;

    if (triangleHeight <= A->rad)
    {
      return true;
    }
    return false;
  }

  bool circleRectCollision(const circle *A, const rect *B)
  {
    // check collision upper border of rect
    /*if (circleLineCollision(A, B->left_upper, B->right_upper))
    {
      return true;
    }
    else if (circleLineCollision(A, B->right_upper, B->right_lower))
    {
      return true;
    }
    else if (circleLineCollision(A, B->right_lower, B->left_lower))
    {
      return true;
    }
    else if (circleLineCollision(A, B->left_lower, B->left_upper))
    {
      return true;
    }
    return false;*/

    float circle_rotation_angle = -B->rotation;

    float anchor_x = 0;
    float anchor_y = 0;
    switch (B->anchor)
    {
    case LEFT_UPPER_CORNER:
      anchor_x = B->pos.x;
      anchor_y = B->pos.y;
      break;
    case LEFT_LOWER_CORNER:
      anchor_x = B->pos.x;
      anchor_y = B->pos.y + B->height;
      break;
    case RIGHT_UPPER_CORNER:
      anchor_x = B->pos.x + B->width;
      anchor_y = B->pos.y;
      break;
    case RIGHT_LOWER_CORNER:
      anchor_x = B->pos.x + B->width;
      anchor_y = B->pos.y + B->height;
      break;
    default: // center
      anchor_x = B->pos.x + B->width / 2;
      anchor_y = B->pos.y + B->height / 2;
      break;
    }
    vec2f rotation_anchor = vec2f(anchor_x, anchor_y);

    vec2f unrotatedCircle = rotatePoint(A->pos, rotation_anchor, circle_rotation_angle);

    // assume unrotated circle and rectangle and calculate collision on unrotated rectangle and circle
    // i.e. if rectangle rotated by 45 deg -> rotate circle by -45 deg and calculate collision between rotated circle and unrotated rectangle

    float x_near = std::clamp(unrotatedCircle.x, B->pos.x, B->pos.x + B->width);
    float y_near = std::clamp(unrotatedCircle.y, B->pos.y, B->pos.y + B->height);

    vec2f circle_rect_dist = vec2f(unrotatedCircle.x - x_near, unrotatedCircle.y - y_near);

    // If dist < circle radius -> intersection = true
    float dist_squared = (circle_rect_dist.x * circle_rect_dist.x) + (circle_rect_dist.y * circle_rect_dist.y);
    return dist_squared < (A->rad * A->rad);

    /*float x_near = std::clamp(A->pos.x, B->pos.x, B->pos.x + B->width);
    float y_near = std::clamp(A->pos.y, B->pos.y, B->pos.y + B->height);

    vec2f circle_rect_dist = vec2f(A->pos.x - x_near, A->pos.y - y_near);

    // If dist < circle radius -> intersection = true
    float dist_squared = (circle_rect_dist.x * circle_rect_dist.x) + (circle_rect_dist.y * circle_rect_dist.y);
    return dist_squared < (A->rad * A->rad);*/

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