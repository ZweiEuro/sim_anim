#include "GameObjects/RigidBody.hpp"
#include "GameCore/GameManager.hpp"
#include "Rendering/SettingsGui.hpp"

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

        ALLEGRO_MOUSE_STATE state;
        al_get_mouse_state(&state);
        al_draw_line(circle::pos.x, circle::pos.y, state.x, state.y, al_map_rgb(255, 0, 0), 2);

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
        if (GameManager::instance()->debug_enabled && this->m_gameobject_type == TYPE_TABLE_BORDER)
        {
          al_draw_rectangle(rect::pos.x, rect::pos.y, rect::pos.x + rect::width, rect::pos.y + rect::height, al_map_rgba(255, 255, 255, 127), 1);
        }
        al_use_transform(&original);
        // al_draw_filled_rectangle(rect::pos.x, rect::pos.y, rect::right_lower.x, rect::right_lower.y, m_color);
        /*if (rect::rotation == 135.0f)
        {
          ALLEGRO_FONT *font = al_create_builtin_font();
          std::string s = "left upper x: ";
          std::string d = ", y: ";
          std::string s2 = ", right upper x: ";
          std::string d2 = ", y: ";
          std::string msg = s + std::to_string(rect::left_upper.x) + d + std::to_string(rect::left_upper.y) + s2 + std::to_string(rect::right_upper.x) + d2 + std::to_string(rect::right_upper.y);
          al_draw_text(font, al_map_rgb(0, 0, 0), rect::left_upper.x - 250, rect::left_upper.y - 40, 0, msg.c_str());
        }*/
      }
      else
      {
        al_draw_filled_rectangle(rect::pos.x, rect::pos.y, rect::pos.x + rect::width, rect::pos.y + rect::height, m_color);
        if (GameManager::instance()->debug_enabled && this->m_gameobject_type == TYPE_TABLE_BORDER)
        {
          al_draw_rectangle(rect::pos.x, rect::pos.y, rect::pos.x + rect::width, rect::pos.y + rect::height, al_map_rgba(255, 255, 255, 127), 1);
        }
      }

      if (this->m_gameobject_type == TYPE_ICE_RECTANGLE && v)
      {
        v->draw();
      }
    }
    // should only be displayed when DEBUG is on
    if (GameManager::instance()->debug_enabled)
    {
      for (size_t i = 0; i < collision_points.size(); i++)
      {
        al_draw_filled_circle(collision_points[i].x, collision_points[i].y, 2, al_map_rgba(255, 0, 0, 255));
      }

      auto dest = circle::pos + m_velocity;

      al_draw_line(circle::pos.x, circle::pos.y, dest.x, dest.y, al_map_rgba(0, 0, 0, 255), 1);
    }
  }
  void RigidBody::move(vec2f delta_move)
  {
    if (this->m_rigid_body_type == TYPE_BALL)
    {
      if (this->m_gameobject_type == TYPE_SATELLITE_BALL)
      {
        // move according to hierarchical transformation

        circle::setPosition(circle::pos + delta_move);
        GameManager ::instance()->s->updateVelocity(this);
        // this->m_velocity = this->m_velocity - (delta_move * (SettingsGUI::instance()->m_checkbox_table_friction.checked() ? table_friction : 0));
      }
      else
      {
        m_past_positions.emplace_back(circle::pos);
        if (abs(this->m_velocity.mag()) < MOVEMENT_EPSILON)
        {
          this->m_velocity = {0.0f, 0.0f};
        }
        circle::setPosition(circle::pos + delta_move);
        this->m_velocity = this->m_velocity - (delta_move * (SettingsGUI::instance()->m_checkbox_table_friction.checked() ? table_friction : 0));
        if (this->circle::pos.x < inner_border_x_offset - this->circle::rad / 2 || this->circle::pos.x > (outer_border_x_offset + pool_table_width - table_border_width + this->circle::rad / 2) || this->circle::pos.y < inner_border_y_offset - this->circle::rad / 2 || this->circle::pos.y > (outer_border_y_offset + pool_table_height - table_border_width + this->circle::rad / 2))
        {
          // ball outside of table
        }
      }
    }
    if (this->m_rigid_body_type == TYPE_RECTANGLE)
    {
      if (this->m_gameobject_type == TYPE_OBSTACLE_RECTANGLE)
      {
        /*if (this->movementCurve)
        {
          return;
        }*/
      }
      if (this->m_gameobject_type == TYPE_ICE_RECTANGLE && !v)
      {
      }

      if (this->m_gameobject_type == TYPE_ICE_RECTANGLE && v)
      {
        if (GameManager::instance()->voronoi_recalc)
        {
          GameManager::instance()->voronoi_recalc = false;
          v->toggleNoised();
          v->recalc();
        }
      }
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
    //⟨v1−v2,x1−x2⟩ = (v1.x - v2.x) * (x1.x - x2.x) + (v1.y - v2.y) * (x1.y - x2.y)
    //⟨v2−v1,x2−x1⟩ = (v2.x - v1.x) * (x2.x - x1.x) + (v2.y - v1.y) * (x2.y - x1.y)

    //||x1 - x2||² = ⟨x1 - x2,x1 - x2⟩ = (x1.x - x2.x) * (x1.x - x2.x) + (x1.y - x2.y) * (x1.y - x2.y)

    //||x2 - x1||² = ⟨x2−x1,x2−x1⟩ = (x2.x - x1.x) * (x2.x - x1.x) + (x2.y - x1.y) * (x2.y - x1.y)

    vec2f vel1 = (this->circle::pos - otherBall->circle::pos) * (2 * otherBall->m_mass) / (this->m_mass + otherBall->m_mass) * ((this->m_velocity - otherBall->m_velocity).dot(this->circle::pos - otherBall->circle::pos)) / ((this->circle::pos - otherBall->circle::pos).dot(this->circle::pos - otherBall->circle::pos));
    vec2f vel2 = (otherBall->circle::pos - this->circle::pos) * (2 * this->m_mass) / (this->m_mass + otherBall->m_mass) * ((otherBall->m_velocity - this->m_velocity).dot(otherBall->circle::pos - this->circle::pos)) / ((otherBall->circle::pos - this->circle::pos).dot(otherBall->circle::pos - this->circle::pos));
    // spdlog::info("prior: velocity-this x: {}, y: {}, velocity-other x: {}, y: {}, vel1 x: {}, y: {}, vel2 x: {}, y: {}", this->m_velocity.x, this->m_velocity.y, otherBall->m_velocity.x, otherBall->m_velocity.y, vel1.x, vel1.y, vel2.x, vel2.y);

    vec2f OtherCenterToThisCenter = this->circle::pos - otherBall->circle::pos;
    // fix the penetration of the one ball into the other -> leads to constant collision
    // fix by increasing the ball center - ball center distance to 2 * ball radius
    vec2f OtherCenterToThisCenterFixedOverlap = OtherCenterToThisCenter * ((this->circle::rad * 2.0f) / OtherCenterToThisCenter.mag());

    this->circle::pos = otherBall->circle::pos + OtherCenterToThisCenterFixedOverlap;

    if (this->m_gameobject_type == TYPE_SATELLITE_BALL)
    {
      otherBall->m_velocity = otherBall->m_velocity - vel2 * this->m_restitution_coeff;
      return;
    }
    if (otherBall->m_gameobject_type == TYPE_SATELLITE_BALL)
    {
      this->m_velocity = this->m_velocity - vel1 * otherBall->m_restitution_coeff;
      return;
    }

    this->m_velocity = this->m_velocity - vel1 * otherBall->m_restitution_coeff;
    otherBall->m_velocity = otherBall->m_velocity - vel2 * this->m_restitution_coeff;
    // spdlog::info("after: velocity-this x: {}, y: {}, velocity-other x: {}, y: {}", this->m_velocity.x, this->m_velocity.y, otherBall->m_velocity.x, otherBall->m_velocity.y);
  }

  bool is_val_within_bound(float x, float lower_bound, float upper_bound)
  {
    return (x >= lower_bound && x <= upper_bound);
  }

  void RigidBody::handle_ball_rectangle_collision(RigidBody *ball, RigidBody *border)
  {
    // rotate circle back -> to calculate collision between circle and axis aligned rectangle
    // rotate circle velocity vector back
    // calculate & resolve collision on axis aligned rectangle
    // rotate circle velocity vector back to its original direction

    if (ball->m_gameobject_type == TYPE_SATELLITE_BALL)
    {
      // collision with rectangle not nice
      return;
    }

    if (border->m_gameobject_type == TYPE_ICE_RECTANGLE && v)
    {
      v->fracture();
    }
    if (border->m_gameobject_type == TYPE_ICE_RECTANGLE && !v)
    {
      v = new VoronoiFracture(dynamic_cast<rect *>(this));
      v->fracture();
    }

    vec2f collision_plane_normal = vec2f(0, 0);

    float anchor_x = 0;
    float anchor_y = 0;
    switch (border->rect::anchor)
    {
    case LEFT_UPPER_CORNER:
      anchor_x = border->rect::pos.x;
      anchor_y = border->rect::pos.y;
      break;
    case LEFT_LOWER_CORNER:
      anchor_x = border->rect::pos.x;
      anchor_y = border->rect::pos.y + border->rect::height;
      break;
    case RIGHT_UPPER_CORNER:
      anchor_x = border->rect::pos.x + border->rect::width;
      anchor_y = border->rect::pos.y;
      break;
    case RIGHT_LOWER_CORNER:
      anchor_x = border->rect::pos.x + border->rect::width;
      anchor_y = border->rect::pos.y + border->rect::height;
      break;
    default: // center
      anchor_x = border->rect::pos.x + border->rect::width / 2;
      anchor_y = border->rect::pos.y + border->rect::height / 2;
      break;
    }
    vec2f rotation_anchor = vec2f(anchor_x, anchor_y);

    float circle_rotation_angle = -border->rect::rotation;

    vec2f unrotatedCircle = rotatePoint(ball->circle::pos, rotation_anchor, circle_rotation_angle);
    vec2f unrotatedVelocityVec = rotatePoint(ball->m_velocity, rotation_anchor, circle_rotation_angle);

    float x_near = std::clamp(unrotatedCircle.x, border->rect::pos.x, border->rect::pos.x + border->rect::width);
    float y_near = std::clamp(unrotatedCircle.y, border->rect::pos.y, border->rect::pos.y + border->rect::height);

    // add collision point -> for visualization purposes
    collision_points.push_back(rotatePoint(vec2f(x_near, y_near), rotation_anchor, -circle_rotation_angle));

    float x_col = std::clamp(ball->circle::pos.x, border->rect::left_upper().x, border->rect::right_upper().x);
    float y_col = std::clamp(ball->circle::pos.y, border->rect::left_upper().y, border->rect::left_lower().y);

    vec2f col_point = rotatePoint(vec2f(x_near, y_near), rotation_anchor, -circle_rotation_angle);

    vec2f BallCenterToColPoint = col_point - ball->circle::pos;
    // add 0.1 to the radius to avoid numerical error where the ball is stuck in the wall
    vec2f BallCenterToBallBorder = BallCenterToColPoint * ((ball->circle::rad + 0.1) / BallCenterToColPoint.mag());
    // spdlog::info("ball center length {}, ball border length {}", BallCenterToColPoint.mag(), BallCenterToBallBorder.mag());
    vec2f BorderPenetration = BallCenterToBallBorder - BallCenterToColPoint;

    vec2f collided_corner(-1, -1); // none

    if ((col_point.x == left_upper().x || is_val_within_bound(col_point.x, left_lower().x, left_upper().x) || is_val_within_bound(col_point.x, left_upper().x, left_lower().x)) && (is_val_within_bound(col_point.y, left_upper().y, left_lower().y) || is_val_within_bound(col_point.y, left_lower().y, left_upper().y)))
    {
      // left edge
      // spdlog::info("left edge collision" /*, al_keycode_to_name(keycode)*/);
      float dx = border->rect::left_upper().x - border->rect::left_lower().x;
      float dy = border->rect::left_upper().y - border->rect::left_lower().y;
      collision_plane_normal = vec2f(dy * -1, dx);
      // spdlog::info("col point x: {}, y: {}; left upper x: {}, y: {}; left lower x: {}, y: {}", col_point.x, col_point.y, left_upper().x, left_upper().y, left_lower().x, left_lower().y);

      if (ball->circle::point_inside(border->rect::left_upper()))
      {
        collided_corner = border->rect::left_upper();
      }
      else if (ball->circle::point_inside(border->rect::left_lower()))
      {
        collided_corner = border->rect::left_lower();
      }
    }
    else if ((col_point.x == right_upper().x || is_val_within_bound(col_point.x, right_lower().x, right_upper().x) || is_val_within_bound(col_point.x, right_upper().x, right_lower().x)) && (is_val_within_bound(col_point.y, right_upper().y, right_lower().y) || is_val_within_bound(col_point.y, right_lower().y, right_upper().y)))
    {
      // right edge
      // spdlog::info("right edge collision" /*, al_keycode_to_name(keycode)*/);
      float dx = border->rect::right_upper().x - border->rect::right_lower().x;
      float dy = border->rect::right_upper().y - border->rect::right_lower().y;
      collision_plane_normal = vec2f(dy * -1, dx);
      // spdlog::info("col point x: {}, y: {}; right upper x: {}, y: {}; right lower x: {}, y: {}", col_point.x, col_point.y, right_upper().x, right_upper().y, right_lower().x, right_lower().y);

      if (ball->circle::point_inside(border->rect::right_upper()))
      {
        collided_corner = border->rect::right_upper();
      }
      else if (ball->circle::point_inside(border->rect::right_lower()))
      {
        collided_corner = border->rect::right_lower();
      }
    }
    else if ((col_point.y == left_upper().y || is_val_within_bound(col_point.y, right_upper().y, left_upper().y) || is_val_within_bound(col_point.y, left_upper().y, right_upper().y)) && (is_val_within_bound(col_point.x, right_upper().x, left_upper().x) || is_val_within_bound(col_point.x, left_upper().x, right_upper().x)))
    {
      // upper edge
      // spdlog::info("upper edge collision" /*, al_keycode_to_name(keycode)*/);
      float dx = border->rect::left_upper().x - border->rect::right_upper().x;
      float dy = border->rect::left_upper().y - border->rect::right_upper().y;
      collision_plane_normal = vec2f(dy * -1, dx);
      // spdlog::info("col point x: {}, y: {}; left upper x: {}, y: {}; right upper x: {}, y: {}", col_point.x, col_point.y, left_upper().x, left_upper().y, right_upper().x, right_upper().y);

      if (ball->circle::point_inside(border->rect::left_upper()))
      {
        collided_corner = border->rect::left_upper();
      }
      else if (ball->circle::point_inside(border->rect::right_upper()))
      {
        collided_corner = border->rect::right_upper();
      }
    }
    else if ((col_point.y == left_lower().y || is_val_within_bound(col_point.y, right_lower().y, left_lower().y) || is_val_within_bound(col_point.y, left_lower().y, right_lower().y)) && (is_val_within_bound(col_point.x, right_lower().x, left_lower().x) || is_val_within_bound(col_point.x, left_lower().x, right_lower().x)))
    {
      // lower edge
      // spdlog::info("lower edge collision" /*, al_keycode_to_name(keycode)*/);
      float dx = border->rect::left_lower().x - border->rect::right_lower().x;
      float dy = border->rect::left_lower().y - border->rect::right_lower().y;
      collision_plane_normal = vec2f(dy * -1, dx);
      // spdlog::info("col point x: {}, y: {}; left lower x: {}, y: {}; right lower x: {}, y: {}", col_point.x, col_point.y, left_lower().x, left_lower().y, right_lower().x, right_lower().y);

      if (ball->circle::point_inside(border->rect::left_lower()))
      {
        collided_corner = border->rect::left_lower();
      }
      else if (ball->circle::point_inside(border->rect::right_lower()))
      {
        collided_corner = border->rect::right_lower();
      }
    }

    if (std::isnan(BorderPenetration.x) || std::isnan(BorderPenetration.y)) // happens if the border penetration == ball radius i.e. when col point == ball center
    {
      // it is still possible for balls to skip into and through rectangles, but this seems unfixable in this implementation
      spdlog::info("clipping: ball penetration == ball radius: ball pos x: {}, y: {}, borderPenetration x: {}, y:{}, col point x: {}, y:{}, collision normal: x: {}, y:{}", ball->circle::pos.x, ball->circle::pos.y, BorderPenetration.x, BorderPenetration.y, col_point.x, col_point.y, collision_plane_normal.x, collision_plane_normal.y);
      if (!std::isnan(collision_plane_normal.x) && !std::isnan(collision_plane_normal.y))
      {
        // use the collision_plane_normal vector to move the ball away from the border by radius + 0.1
        BorderPenetration = collision_plane_normal * ((ball->circle::rad + 0.1) / collision_plane_normal.mag());
        BorderPenetration = BorderPenetration * -1; // as the normal points in the other direction as the usual borderPenetration vector
      }
      else
      {
        // use the velocity vector to move the ball away from the border by radius + 0.1
        BorderPenetration = ball->m_velocity * ((ball->circle::rad + 0.1) / ball->m_velocity.mag());
      }

      ball->circle::pos = ball->circle::pos - (BorderPenetration);
      ball->m_velocity = vec2f(0, 0);
      if (ball->m_gameobject_type == TYPE_BLACK_BALL)
      {
        spdlog::info("Black ball clipped through table border - removing object");
        if (GameManager::instance()->player1_active) // was switched after white ball has been played i.e. if player1_active = true -> last play was by player 2
        {
          if (GameManager::instance()->player2_ball_count == 0)
          {
            spdlog::info("Player 2 won");
            GameManager::instance()->player2_ball_count = -1;
          }
          else
          {
            spdlog::info("Player 2 lost");
            GameManager::instance()->player2_ball_count = -2;
          }
        }
        else
        {
          if (GameManager::instance()->player1_ball_count == 0)
          {
            spdlog::info("Player 1 won");
            GameManager::instance()->player1_ball_count = -1;
          }
          else
          {
            spdlog::info("Player 1 lost");
            GameManager::instance()->player1_ball_count = -2;
          }
        }
      }
      else if (ball->m_gameobject_type == TYPE_PLAYER1_BALL)
      {
        spdlog::info("Player 1 ball clipped through table border - removing object");
        GameManager::instance()->player1_ball_count--;
      }
      else
      {
        spdlog::info("Player 2 ball clipped through table border - removing object");
        GameManager::instance()->player2_ball_count--;
      }
      auto &objects = GameManager::instance()->getGameObjects();
      objects.erase(std::remove(objects.begin(), objects.end(), ball), objects.end());
      GameManager::instance()->releaseGameObjects(true);
      return;
    }

    ball->circle::pos = ball->circle::pos - (BorderPenetration);
    // spdlog::info("ball pos x: {}, y: {}, borderPenetration x: {}, y:{}, col point x: {}, y:{}", ball->circle::pos.x, ball->circle::pos.y, BorderPenetration.x, BorderPenetration.y, col_point.x, col_point.y);

    if (collided_corner != vec2f(-1, -1))
    {
      spdlog::info("direct corner collision");

      ball->m_velocity = ball->m_velocity * -1 * border->m_restitution_coeff;
      return;
    }

    if (collision_plane_normal.x == 0 && collision_plane_normal.y == 0)
    {
      return;
      assert(false && "normal is zero");
    }

    collision_plane_normal = collision_plane_normal.dir();
    vec2f velocity_ = ball->m_velocity - collision_plane_normal * (ball->m_velocity.dot(collision_plane_normal)) * 2 * border->m_restitution_coeff;
    ball->m_velocity = velocity_;
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