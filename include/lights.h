#pragma once

#include "vector.h"
#include <stdint.h>
#define MAX_NUMBER_OF_LIGHTS 10

typedef struct {
  vec3_t position;
  uint32_t color;
} light_t;

void init_lights_in_scene(light_t lights[], int *number_of_lights);
vec3_t light_reflect(vec3_t light_direction, vec3_t normal);
uint32_t light_mix_color(uint32_t color1, uint32_t color2,
                         float interpolation_factor);
