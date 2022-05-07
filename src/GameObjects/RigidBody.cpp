#include "GameObjects/RigidBody.hpp"
#include "GameCore/GameManager.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "configuration.hpp"
#include "math/math.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <string>

namespace mg8
{
  void RigidBody::draw() const
  {
    if (this->m_rigid_body_type == TYPE_BALL)
    {
      al_draw_filled_circle(circle::pos.x, circle::pos.y, rad, al_map_rgba(m_color.r, m_color.g, m_color.b, m_color.a));
      if (this->m_gameobject_type == TYPE_WHITE_BALL)
      {
        ALLEGRO_FONT *font = al_create_builtin_font();
        std::string s = "x: ";
        std::string d = ", y: ";
        std::string msg = s + std::to_string(this->m_velocity.x) + d + std::to_string(this->m_velocity.y);
        al_draw_text(font, al_map_rgb(0, 0, 0), circle::pos.x + rad * 3, circle::pos.y + rad * 3, 0, msg.c_str());
      }
    }
    if (this->m_rigid_body_type == TYPE_RECTANGLE)
    {
      if (rect::rotation != 0.0f)
      {
        ALLEGRO_TRANSFORM t, original;
        t = *al_get_current_transform();
        original = t;

        float anchor_x = 0;
        float anchor_y = 0;
        switch (rect::anchor)
        {
        case LEFT_UPPER_CORNER:
          anchor_x = rect::pos.x;
          anchor_y = rect::pos.y;
          break;
        case LEFT_LOWER_CORNER:
          anchor_x = rect::pos.x;
          anchor_y = rect::pos.y + rect::height;
          break;
        case RIGHT_UPPER_CORNER:
          anchor_x = rect::pos.x + rect::width;
          anchor_y = rect::pos.y;
          break;
        case RIGHT_LOWER_CORNER:
          anchor_x = rect::pos.x + rect::width;
          anchor_y = rect::pos.y + rect::height;
          break;
        default: // center
          anchor_x = rect::pos.x + rect::width / 2;
          anchor_y = rect::pos.y + rect::height / 2;
          break;
        }
        al_translate_transform(&t, -anchor_x, -anchor_y);       // set rotation anchor
        al_rotate_transform(&t, rect::rotation * M_PI / 180.0); // rotate
        al_translate_transform(&t, anchor_x, anchor_y);
        al_use_transform(&t);
        al_draw_filled_rectangle(rect::pos.x, rect::pos.y, rect::pos.x + rect::width, rect::pos.y + rect::height, m_color);
        al_use_transform(&original);
      }
      else
      {
        al_draw_filled_rectangle(rect::pos.x, rect::pos.y, rect::pos.x + rect::width, rect::pos.y + rect::height, m_color);
      }
    }
  }
  void RigidBody::move(vec2f delta_move)
  {
    if (this->m_rigid_body_type == TYPE_BALL)
    {
      if (this->m_velocity.x < MOVEMENT_EPSILON && this->m_velocity.y < MOVEMENT_EPSILON)
      {
        this->m_velocity = {0.0f, 0.0f};
      }
      circle::setPosition(circle::pos + delta_move);
      this->m_velocity = this->m_velocity - (this->m_velocity * table_friction);
    }
    if (this->m_rigid_body_type == TYPE_RECTANGLE)
    {
      return;
    }
  }

  void RigidBody::handle_collision(RigidBody *collisioner)
  {
    if (this->m_rigid_body_type == TYPE_BALL && collisioner->m_rigid_body_type == TYPE_BALL)
    {
      handle_ball_ball_collision(collisioner);
    }
    else if (this->m_rigid_body_type == TYPE_BALL && collisioner->m_rigid_body_type == TYPE_RECTANGLE)
    {
      handle_ball_rectangle_collision(this, collisioner);
    }
    else if (this->m_rigid_body_type == TYPE_RECTANGLE && collisioner->m_rigid_body_type == TYPE_BALL)
    {
      handle_ball_rectangle_collision(collisioner, this);
    }
    else
    {
      // assert(false && "Border - border collision should not happen.");
    }
  }

  void RigidBody::handle_ball_ball_collision(RigidBody *otherBall)
  {
    vec2f pos_delta = this->circle::pos - otherBall->circle::pos;
    float r = this->rad + otherBall->rad;

    float d = pos_delta.mag();

    if (d == 0.0f)
    {
      // special case - balls are exactly on top of each other
      d = r - 1.0f;
      pos_delta = vec2f(r, 0.0f);
    }
    vec2f min_translational_dist = pos_delta * ((r - d) / d);

    float inverseMass_self = 1 / this->m_mass;
    float inverseMass_other = 1 / otherBall->m_mass;

    this->circle::pos = this->circle::pos + min_translational_dist * (inverseMass_self / (inverseMass_self + inverseMass_other));
    otherBall->circle::pos = otherBall->circle::pos - min_translational_dist * (inverseMass_other / (inverseMass_self + inverseMass_other));

    vec2f velocity = this->m_velocity - otherBall->m_velocity;
    float vn = velocity.dot(min_translational_dist.dir());
    if (vn > 0.0f)
    {
      // balls are already moving apart
      return;
    }
    float i = (-(1.0f + this->m_restitution_coeff) * vn) / (inverseMass_self + inverseMass_other);
    vec2f impulse = min_translational_dist * i;

    this->m_velocity = this->m_velocity + impulse * inverseMass_self;
    otherBall->m_velocity = otherBall->m_velocity - impulse * inverseMass_other;
  }

  void RigidBody::handle_ball_rectangle_collision(RigidBody *ball, RigidBody *border)
  {

    vec2f normal_directions[] = {
        vec2f(0.0f, 1.0f),  // down = 0
        vec2f(1.0f, 0.0f),  // right = 1
        vec2f(0.0f, -1.0f), // up = 2
        vec2f(-1.0f, 0.0f)  // left = 3
    };

    float x_near = std::clamp(ball->circle::pos.x, border->rect::pos.x, border->rect::pos.x + border->rect::width);
    float y_near = std::clamp(ball->circle::pos.y, border->rect::pos.y, border->rect::pos.y + border->rect::height);

    vec2f v = vec2f(x_near - ball->circle::pos.x, y_near - ball->circle::pos.y);
    float border_penetration = ball->circle::rad - v.mag();
    vec2f r = v.dir() * border_penetration;

    float max = 0.0f;
    int best_match = -1;
    for (size_t i = 0; i < 4; i++)
    {
      float dot_prod = r.dir().dot(normal_directions[i]);
      if (dot_prod > max)
      {
        max = dot_prod;
        best_match = i;
      }
    }

    if (best_match == 1 || best_match == 3) // horizontal collision
    {
      ball->m_velocity.x = ball->m_velocity.x * -1 * border->m_restitution_coeff;
      ball->m_velocity.y = ball->m_velocity.y * border->m_restitution_coeff;
      if (best_match == 1) // right collision - move ball left
      {
        ball->circle::pos.x -= border_penetration;
      }
      else // left collision - move ball right
      {
        ball->circle::pos.x += border_penetration;
      }
    }
    else // vertical collision
    {
      ball->m_velocity.y = ball->m_velocity.y * -1 * border->m_restitution_coeff;
      ball->m_velocity.x = ball->m_velocity.x * border->m_restitution_coeff;
      if (best_match == 0) // down collision - move ball up
      {
        ball->circle::pos.y -= border_penetration;
      }
      else // up collision - move ball down
      {
        ball->circle::pos.y += border_penetration;
      }
    }

    /*
    float x_near = std::clamp(ball->circle::pos.x, border->rect::pos.x, border->rect::pos.x + border->rect::width);
    float y_near = std::clamp(ball->circle::pos.y, border->rect::pos.y, border->rect::pos.y + border->rect::height);

    vec2f ball_border_dist = vec2f(ball->circle::pos.x - x_near, ball->circle::pos.y - y_near);
    vec2f ball_border_dist_normalized = ball_border_dist.dir();
    vec2f ball_border_dist_perpendicular = vec2f(-ball_border_dist_normalized.y, ball_border_dist_normalized.x);

    if (ball->m_velocity.dot(ball_border_dist) < 0)
    {
      vec2f normal_vec_len = ball_border_dist_normalized.dot(ball->m_velocity);
      vec2f tangental_vec_len = ball_border_dist_perpendicular.dot(ball->m_velocity);

      vec2f part1 = (tangental_vec_len * border->m_restitution_coeff).dot(ball_border_dist_perpendicular);
      vec2f part2 = ((ball_border_dist_normalized * -1.0f) * ball->m_restitution_coeff).dot(normal_vec_len);

      ball->m_velocity = part1 + part2;
    }*/

    /*float x_near = std::clamp(ball->circle::pos.x, border->rect::pos.x, border->rect::pos.x + border->rect::width);
    float y_near = std::clamp(ball->circle::pos.y, border->rect::pos.y, border->rect::pos.y + border->rect::height);

    vec2f ball_border_dist = vec2f(ball->circle::pos.x - x_near, ball->circle::pos.y - y_near);

    if (ball->m_velocity.dot(ball_border_dist) < 0)
    { // ball is moving towards border
      vec2f tangent_velocity = ball_border_dist.dir().dot(ball->m_velocity);
      ball->m_velocity = ball->m_velocity - tangent_velocity * 2;
    }

    float border_penetration = ball->circle::rad - ball_border_dist.mag();
    vec2f border_penetration_vector = ball_border_dist.dir() * border_penetration;
    ball->circle::pos = ball->circle::pos - border_penetration_vector;*/
  }

  // BilliardBall constructor
  RigidBody::RigidBody(MG8_RIGID_BODY_OBJECT_TYPES rigid_body_type,
                       MG8_GAMEOBJECT_TYPES gameobject_type,
                       vec2f position,
                       vec2f velocity,
                       int radius,
                       vec2f acceleration,
                       float mass,
                       float restitution_coeff,
                       ALLEGRO_COLOR color,
                       uint32_t collision,
                       MG8_OBJECT_TYPES obj_type) : GameObject(obj_type, collision, velocity),
                                                    circle(position, radius),
                                                    m_color(color), m_acceleration(acceleration), m_mass(mass), m_restitution_coeff(restitution_coeff), m_rigid_body_type(rigid_body_type), m_gameobject_type(gameobject_type)
  {
  }
  // TableBorder constructor
  RigidBody::RigidBody(MG8_RIGID_BODY_OBJECT_TYPES rigid_body_type,
                       MG8_GAMEOBJECT_TYPES gameobject_type,
                       vec2f position,
                       vec2f velocity,
                       float width,
                       float height,
                       float rotation,
                       MG8_ROTATION_ANCHOR rotation_anchor,
                       vec2f acceleration,
                       float mass,
                       float restitution_coeff,
                       ALLEGRO_COLOR color,
                       uint32_t collision,
                       MG8_OBJECT_TYPES obj_type) : GameObject(obj_type, collision, velocity),
                                                    rect(position, width, height, rotation, rotation_anchor),
                                                    m_color(color), m_acceleration(acceleration), m_mass(mass), m_restitution_coeff(restitution_coeff), m_rigid_body_type(rigid_body_type), m_gameobject_type(gameobject_type)
  {
  }
}