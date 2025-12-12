#include "matrix.h"
#include "vector.h"
#include <math.h>

mat4_t mat4_make_identity(void) {
  mat4_t matrix = {
      .data = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
  return matrix;
}

mat4_t mat4_make_rotation_y(float angle) {
  mat4_t m = mat4_make_identity();

  float cos_angle = cos(angle);
  float sin_angle = sin(angle);

  m.data[0][0] = cos_angle;
  m.data[0][2] = sin_angle;
  m.data[2][0] = -sin_angle;
  m.data[2][2] = cos_angle;

  return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
  vec4_t result = {.x = m.data[0][0] * v.x + m.data[0][1] * v.y +
                        m.data[0][2] * v.z + m.data[0][3] * v.w,

                   .y = m.data[1][0] * v.x + m.data[1][1] * v.y +
                        m.data[1][2] * v.z + m.data[1][3] * v.w,

                   .z = m.data[2][0] * v.x + m.data[2][1] * v.y +
                        m.data[2][2] * v.z + m.data[2][3] * v.w,

                   .w = m.data[3][0] * v.x + m.data[3][1] * v.y +
                        m.data[3][2] * v.z + m.data[3][3] * v.w};
  return result;
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {
  mat4_t m;

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      m.data[i][j] = a.data[i][0] * b.data[0][j] + a.data[i][1] * b.data[1][j] +
                     a.data[i][2] * b.data[2][j] + a.data[i][3] * b.data[3][j];
    };
  }

  return m;
}
