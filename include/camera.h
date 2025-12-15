#pragma once

#include "vector.h"

typedef struct {
  vec3_t position;
  vec3_t direction;
  int delta_x_mouse_movement; // left and right movement along a fixed
                              // Y-axis(YAW)
  int delta_y_mouse_movement; // Up and Down rotation along a fixed
                              // X-Axis(PITCH)
} camera_t;

camera_t create_base_camera(void);
