#pragma once
#include "appstate.h"
#include "utilities.h"
#include "vector.h"

typedef struct {
  vec2_t vertices[3];
  color_t colors[3];
} triangle_t;

void fill_triangle(triangle_t triangle, app_state_t *app_state);
