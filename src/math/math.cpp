#include "math/math.hpp"
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
  float dot(vec2f a, vec2f b) { return a.x * b.x + a.y * b.y; }

  bool circleCircleCollision(const circle *A, const circle *B)
  {
    assert(A && B && "Circle circle collision with nullptr");
    vec2f c = sub(B->pos, A->pos);
    float d2 = dot(c, c);
    float r2 = A->rad + B->rad;
    r2 = r2 * r2;
    return d2 < r2;
  }

  bool circleRectCollision(const circle *A, const rect *B)
  {
    assert(A && B && "Circle poly collision with nullptr");
    return false;
    // https://stackoverflow.com/a/402019/12258809

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
}