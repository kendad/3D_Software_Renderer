#pragma once

#include "vector.h"

typedef struct {
  float data[4][4];
} mat4_t;

mat4_t mat4_make_identity();

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);
mat4_t mat4_mul_mat4(mat4_t a, mat4_t b);
