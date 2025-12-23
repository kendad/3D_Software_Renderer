#include "utilities.h"
#include <stdint.h>

float min(float a, float b) { return (a < b) ? a : b; }
float max(float a, float b) { return (a > b) ? a : b; }
float lerp(float a, float b, float t) { return a + (t * (b - a)); }
uint32_t lerp_uint32(uint32_t a, uint32_t b, float t) {
  return (uint32_t)(a + t * ((float)b - (float)a));
}
float clamp(float v, float min, float max) {
  float clamped = v < min ? min : v;
  return clamped > max ? max : clamped;
}

// converts the 8bit rgba color channels to 32bit color variable
uint32_t create_color_uint32(color_t color) {
  uint32_t color_uint32 = ((uint32_t)color.a << 24) |
                          ((uint32_t)color.r << 16) | ((uint32_t)color.g << 8) |
                          (uint32_t)color.b;
  return color_uint32;
}
