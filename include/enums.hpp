#pragma once
#include <allegro5/allegro.h>

// General Enum file that every type should know. Good for inter-thread communication or return type for functions

namespace mg8
{
#define USER_BASE_EVENT ALLEGRO_GET_EVENT_TYPE('m', 'g', '8', '_')

  // enum classes are _NOT CASTABLE_

  enum MG8_EVENTS
  {
    CONTROL_SHUTDOWN = ALLEGRO_GET_EVENT_TYPE('m', 'g', '8', '_') + 1
  };

  enum MG8_SUBSYSTEMS
  {
    GAMEMANAGER = 0,
    INPUT_MANAGER,
    PHYSICS_MANAGER,
    RENDERER,
  };
}
