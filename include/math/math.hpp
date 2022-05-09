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

    template <typename T2>
    vec2 operator/(T2 divisor)
    {
      return {(T)((double)x / (double)divisor), (T)((double)y / (double)divisor)};
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

    float mag() const
    {
      return std::sqrt(x * x + y * y);
    }

    float dot(vec2 other) const
    {
      return x * other.x + y * other.y;
    }

    vec2<float> dir() const // i.e. normalize
    {
      return {((float)x) / mag(), ((float)y) / mag()};
    }
  };
  using vec2f = vec2<float>;
  using vec2i = vec2<int>;

  vec2f rotatePoint(const vec2f, const vec2f, const float);

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
    vec2f left_upper;
    vec2f left_lower;
    vec2f right_upper;
    vec2f right_lower;

    rect()
    {
    }

    rect(vec2f pos, float width, float height, float rotation, MG8_ROTATION_ANCHOR anchor) : pos(pos), width(width), height(height), rotation(rotation), anchor(anchor)
    {
      float anchor_x = 0;
      float anchor_y = 0;
      switch (anchor)
      {
      case LEFT_UPPER_CORNER:
        anchor_x = pos.x;
        anchor_y = pos.y;
        break;
      case LEFT_LOWER_CORNER:
        anchor_x = pos.x;
        anchor_y = pos.y + height;
        break;
      case RIGHT_UPPER_CORNER:
        anchor_x = pos.x + width;
        anchor_y = pos.y;
        break;
      case RIGHT_LOWER_CORNER:
        anchor_x = pos.x + width;
        anchor_y = pos.y + height;
        break;
      default: // center
        anchor_x = pos.x + width / 2;
        anchor_y = pos.y + height / 2;
        break;
      }
      vec2f rotation_anchor = vec2f(anchor_x, anchor_y);

      left_upper = rotatePoint(pos, rotation_anchor, rotation);
      left_lower = rotatePoint(vec2f(pos.x, pos.y + height), rotation_anchor, rotation);
      right_upper = rotatePoint(vec2f(pos.x + width, pos.y), rotation_anchor, rotation);
      right_lower = rotatePoint(vec2f(pos.x + width, pos.y + height), rotation_anchor, rotation);
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
  bool circleLineCollision(const circle *A, vec2f P, vec2f Q);
}