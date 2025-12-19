#pragma once

#include <stdint.h>

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} color_t;

float min(float a, float b);
float max(float a, float b);
float lerp(float a, float b, float t);
