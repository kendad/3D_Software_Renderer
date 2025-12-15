#pragma once

#include "vector.h"

typedef struct {
  vec3_t position;
  vec3_t direction;
} camera_t;

camera_t create_base_camera(void);
