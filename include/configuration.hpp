#pragma once
#include <stddef.h>
// static configuration for some settings that are set by compile time

namespace mg8
{
  // display
  const float config_default_fps = 60;
  const int config_start_resolution_w = 1080;
  const int config_start_resolution_h = 720;
  const bool config_allow_resize = false;
  // input
  const float config_mouse_position_refresh_per_second = 5;

  // Physics:
  const float config_default_pps = 80;
  const float MOVEMENT_EPSILON = 1;
  const float table_friction = 0.75;

  const float config_default_white_ball_power = 500;

  const float config_default_h = 0.0025;
  const bool config_default_gravity = true; // True = Euler, false = kutta

  const float config_max_velocity_x = 200;
  const float config_max_velocity_y = 200;

  const float config_forcefield_grid_dims = 50;
  const int config_max_number_grav_wells = 4;

  const int config_show_last_X_positions = 10;

  // Table Dimensions:
  const float hole_radius = 10.0;
  const float table_border_width = 40;

  const float outer_border_x_offset = 60;
  const float inner_border_x_offset = outer_border_x_offset + table_border_width;

  const float pool_table_width = (float)config_start_resolution_w - 2 * outer_border_x_offset;
  const float pool_table_height = pool_table_width / 2.0;

  const float outer_border_y_offset = ((float)config_start_resolution_h - pool_table_height) / 2.0;
  const float inner_border_y_offset = outer_border_y_offset + table_border_width;

  // Visual:
  const int config_voronoi_cells = 15;

}