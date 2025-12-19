#pragma once

#include <stdint.h>
typedef struct {
  int width;
  int height;
  int no_of_channels;
  uint32_t *data;
} texture_t;

typedef struct {
  float u;
  float v;
} tex2_t;

texture_t load_texture_data(char *filename);
tex2_t tex2_clone(tex2_t *t);
