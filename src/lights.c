#include "lights.h"
#include "utilities.h"
#include "vector.h"
#include <stdint.h>
#include <stdio.h>

void init_lights_in_scene(light_t *lights, int *number_of_lights) {
  if (*number_of_lights > MAX_NUMBER_OF_LIGHTS)
    return;
  vec3_t light1_pos = {.x = 0, .y = 5.0, .z = 0};
  color_t yellow_light = {.r = 0x00, .g = 0xFF, .b = 0x00, .a = 0xFF};
  light_t light1 = {.position = light1_pos, create_color_uint32(yellow_light)};
  lights[*number_of_lights] = light1;
  (*number_of_lights)++;
}

vec3_t light_reflect(vec3_t light_direction, vec3_t normal) {
  // 2(l.n)n - l-->reflection vector formula

  // 2(l.n)
  float l_dot_n = vec3_dot(light_direction, normal);
  l_dot_n *= 2.0;

  // 2(l.n)n
  vec3_mul(&normal, l_dot_n);

  vec3_t reflection_direction = vec3_sub(normal, light_direction);
  return reflection_direction;
}

uint32_t light_mix(uint32_t color1, uint32_t color2,
                   float interpolation_factor) {
  return lerp_uint32(color1, color2, interpolation_factor);
}
