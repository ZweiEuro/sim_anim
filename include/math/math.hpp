#pragma once
#include "configuration.hpp"
#include "assert.h"
namespace mg8
{

  typedef class vec2
  {
  public:
    float x;
    float y;

    vec2(float x = 0, float y = 0) : x(x), y(y)
    {
    }

    vec2 operator*(float scale)
    {
      return {x * scale, y * scale};
    }

    vec2 operator+(vec2 other)
    {
      return {x + other.x, y + other.y};
    }
  } vec2;

  typedef struct circle
  {
  public:
    vec2 pos;
    float rad;

    circle(vec2 pos, float rad) : pos(pos), rad(rad)
    {
    }

    void setPosition(vec2 new_position)
    {
      pos = new_position;
    }
  } circle;

  typedef struct rect
  {
  public:
    vec2 pos; // upper left corner
    float width;
    float height;

    rect()
    {
      assert(false && "WIP");
    }

    bool point_inside(vec2 point) const;
    void setPosition(vec2 new_position)
    {
      pos = new_position;
    }
  } rect;

  typedef struct ray
  {
  public:
    vec2 pos;       // position
    vec2 dir;       // direction (normalized)
    float distance; // distance along d from position p to find endpoint of ray
  } ray;

  bool circleCircleCollision(const circle *A, const circle *B);
  bool circleRectCollision(const circle *A, const rect *B);
  bool rectRectCollision(const rect *A, const rect *B);
}