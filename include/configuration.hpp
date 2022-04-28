#pragma once
#include <stddef.h>

// static configuration for some settings that are set by compile time

namespace mg8
{

  const int config_max_object_count = 100;
  const float config_fps = 10.0;
  const int config_start_resolution_w = 640;
  const int config_start_resolution_h = 480;

  const float config_mouse_position_refresh_per_second = 5;
}