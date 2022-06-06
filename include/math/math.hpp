#pragma once
#include "configuration.hpp"
#include "assert.h"
#include <cmath>
#include <random>
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
      return {(T)(x * scale), (T)(y * scale)};
    }

    template <typename T2>
    vec2 operator/(T2 divisor)
    {
      return {(T)((double)x / (double)divisor), (T)((double)y / (double)divisor)};
    }

    /*vec2 operator*(float scale)
    {
      return {x * scale, y * scale};
    }*/

    vec2 operator+(const vec2 other) const
    {
      return {x + other.x, y + other.y};
    }

    vec2 operator+=(const vec2 other)
    {

      x += other.x;
      y += other.y;

      return {x, y};
    }

    vec2 operator-(const vec2 other) const
    {
      return {x - other.x, y - other.y};
    }

    bool operator==(const vec2 other) const
    {
      return x == other.x && y == other.y;
    }

    bool operator!=(const vec2 other) const
    {
      return !(this->operator==(other));
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

  float eucledianDistance(const vec2f a, const vec2f b);
  float squaredDistance(const vec2f a, const vec2f b);

  vec2f rotatePoint(const vec2f point, const vec2f anchor_point, const float angle);

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

    bool point_inside(vec2f point) const
    {

      auto mag = (pos - point).mag();

      return mag <= (rad + 0.1f);
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

    vec2f get_anchor()
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
      return vec2f(anchor_x, anchor_y);
    }

    vec2f left_upper()
    {
      return rotatePoint(pos, get_anchor(), rotation);
    }
    vec2f left_lower()
    {
      return rotatePoint(vec2f(pos.x, pos.y + height), get_anchor(), rotation);
    }
    vec2f right_upper()
    {
      return rotatePoint(vec2f(pos.x + width, pos.y), get_anchor(), rotation);
    }
    vec2f right_lower()
    {
      return rotatePoint(vec2f(pos.x + width, pos.y + height), get_anchor(), rotation);
    }

    rect()
    {
    }

    rect(vec2f pos, float width, float height, float rotation, MG8_ROTATION_ANCHOR anchor) : pos(pos), width(width), height(height), rotation(rotation), anchor(anchor)
    {
    }

    vec2f get_random_point_in_rect()
    {
      std::random_device r;
      std::mt19937 gen(r());
      std::uniform_real_distribution<float> dis(0.0f, 1.0f);
      float x = pos.x + ((pos.x + width) - pos.x) * dis(gen);
      float y = pos.y + ((pos.y + height) - pos.y) * dis(gen);

      vec2f rotatedPoint = rotatePoint(vec2f(x, y), get_anchor(), rotation);
      return vec2f(rotatedPoint.x, rotatedPoint.y);
    }

    vec2f get_random_point_in_unrotated_rect()
    {
      std::random_device r;
      std::mt19937 gen(r());
      std::uniform_real_distribution<float> dis(0.0f, 1.0f);
      float x = pos.x + ((pos.x + width) - pos.x) * dis(gen);
      float y = pos.y + ((pos.y + height) - pos.y) * dis(gen);
      return vec2f(x, y);
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
  bool circleHoleCollision(const circle *A, const circle *B);
}