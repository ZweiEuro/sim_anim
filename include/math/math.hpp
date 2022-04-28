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

  } vec2;

  typedef struct circle
  {
  public:
    vec2 pos;
    float rad;

    circle(vec2 pos, float rad) : pos(pos), rad(rad)
    {
    }

  } circle;

  typedef struct poly
  {
  public:
    int count;
    vec2 verts[MAX_POLYGON_VER_COUNT];
    vec2 norms[MAX_POLYGON_VER_COUNT];
    poly()
    {
      assert(false && "WIP");
    }
  } poly;

  typedef struct ray
  {
  public:
    vec2 pos;       // position
    vec2 dir;       // direction (normalized)
    float distance; // distance along d from position p to find endpoint of ray
  } ray;

  int circleCircleCollision(circle A, circle B);
}