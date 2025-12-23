#pragma once

#include <stdint.h>

typedef struct {
  uint8_t a;
  uint8_t b;
  uint8_t g;
  uint8_t r;
} color_t;

float min(float a, float b);
float max(float a, float b);
float lerp(float a, float b, float t);
uint32_t lerp_uint32(uint32_t a, uint32_t b, float t);
float clamp(float v, float min, float max);
uint32_t create_color_uint32(color_t color);
