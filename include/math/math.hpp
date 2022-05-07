#pragma once
#include "configuration.hpp"
#include "assert.h"
#include <cmath>
namespace mg8
{
  enum MG8_ROTATION_ANCHOR
  {
    CENTER = 0,
    LEFT_UPPER_CORNER,
    LEFT_LOWER_CORNER,
    RIGHT_UPPER_CORNER,
    RIGHT_LOWER_CORNER
  };

  template <typename T>
  class vec2
  {
  public:
    T x;
    T y;

    vec2(T x = 0, T y = 0) : x(x), y(y)
    {
    }

    template <typename T2>
    vec2 operator*(T2 scale)
    {
      return {(T)((double)x * (double)scale), (T)((double)y * (double)scale)};
    }

    vec2 operator+(vec2 other)
    {
      return {x + other.x, y + other.y};
    }
    vec2 operator-(vec2 other)
    {
      return {x - other.x, y - other.y};
    }

    template <typename T2>
    operator vec2<T2>() const
    {
      return vec2<T2>((T2)x, (T2)y);
    }

    float mag()
    {
      return std::sqrt(x * x + y * y);
    }

    float dot(vec2 other)
    {
      return x * other.x + y * other.y;
    }

    vec2<float> dir() // i.e. normalize
    {
      return {((float)x) / mag(), ((float)y) / mag()};
    }
  };
  using vec2f = vec2<float>;
  using vec2i = vec2<int>;

  typedef struct circle
  {
  public:
    vec2f pos;
    float rad;

    circle() {}
    circle(vec2f pos, float rad) : pos(pos), rad(rad)
    {
    }

    void setPosition(vec2f new_position)
    {
      pos = new_position;
    }
  } circle;

  typedef struct rect
  {
  public:
    vec2f pos; // upper left corner
    float width;
    float height;
    float rotation;
    MG8_ROTATION_ANCHOR anchor;

    rect()
    {
    }

    rect(vec2f pos, float width, float height, float rotation, MG8_ROTATION_ANCHOR anchor) : pos(pos), width(width), height(height), rotation(rotation), anchor(anchor)
    {
    }

    bool point_inside(vec2f point) const;
    void setPosition(vec2f new_position)
    {
      pos = new_position;
    }
  } rect;

  typedef struct ray
  {
  public:
    vec2f pos;      // position
    vec2f dir;      // direction (normalized)
    float distance; // distance along d from position p to find endpoint of ray
  } ray;

  bool circleCircleCollision(const circle *A, const circle *B);
  bool circleRectCollision(const circle *A, const rect *B);
  bool rectRectCollision(const rect *A, const rect *B);
}