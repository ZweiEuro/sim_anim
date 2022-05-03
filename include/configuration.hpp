#pragma once
#include <stddef.h>

// static configuration for some settings that are set by compile time

namespace mg8
{
  // display
  const float config_fps = 60;
  const int config_start_resolution_w = 1920;
  const int config_start_resolution_h = 1200;
  const bool config_allow_resize = false;
  // input
  const float config_mouse_position_refresh_per_second = 5;

  // Physics:
  const float config_physics_updates_per_second = 80;

}