#include "matrix.h"
#include "vector.h"
#include <math.h>

mat4_t mat4_make_identity(void) {
  mat4_t matrix = {
      .data = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
  return matrix;
}

/*
 *
 *               TRANSFORMATIONS
 *
 * */

/////////////////////// SCALE ///////////////////////////////
mat4_t mat4_make_scale(float sx, float sy, float sz) {
  mat4_t m = mat4_make_identity();
  m.data[0][0] = sx;
  m.data[1][1] = sy;
  m.data[2][2] = sz;

  return m;
}
///////////////////////////////////////////////////////////////

////////////////// ROTATIONS /////////////////////////////////
mat4_t mat4_make_rotation_x(float angle) {
  mat4_t m = mat4_make_identity();

  float cos_angle = cos(angle);
  float sin_angle = sin(angle);

  m.data[1][1] = cos_angle;
  m.data[1][2] = -sin_angle;
  m.data[2][1] = sin_angle;
  m.data[2][2] = cos_angle;

  return m;
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
mat4_t mat4_make_rotation_z(float angle) {
  mat4_t m = mat4_make_identity();

  float cos_angle = cos(angle);
  float sin_angle = sin(angle);

  m.data[0][0] = cos_angle;
  m.data[0][1] = -sin_angle;
  m.data[1][0] = sin_angle;
  m.data[1][1] = cos_angle;

  return m;
}

/////////////////////////////////////////////////////////////////
/////////////////////  TRANSLATIONS /////////////////////////////
mat4_t mat4_make_translation(float tx, float ty, float tz) {
  mat4_t m = mat4_make_identity();
  m.data[0][3] = tx;
  m.data[1][3] = ty;
  m.data[2][3] = tz;
  return m;
}

/////////////////////////////////////////////////////////////////
/////////////////////  VIEW MATRIX //////////////////////////////
mat4_t mat4_make_look_at(vec3_t eye, vec3_t target, vec3_t up) {
  vec3_t z_basis = vec3_sub(target, eye);
  vec3_t x_basis = vec3_cross(up, z_basis);
  vec3_t y_basis = vec3_cross(z_basis, x_basis);

  vec3_normalize(&x_basis);
  vec3_normalize(&y_basis);
  vec3_normalize(&z_basis);

  mat4_t view_matrix = {
      .data = {{x_basis.x, x_basis.y, x_basis.z, -vec3_dot(x_basis, eye)},
               {y_basis.x, y_basis.y, y_basis.z, -vec3_dot(y_basis, eye)},
               {z_basis.x, z_basis.y, z_basis.z, -vec3_dot(z_basis, eye)},
               {0, 0, 0, 1}}};

  return view_matrix;
}

//////////////////////////////////////////////////////////////////////
////////////////////////// PROJECTIONS //////////////////////////////
/////////////////////////////////////////////////////////////////////
mat4_t mat4_make_perspective(float fov, float aspect_ratio, float near,
                             float far) {
  mat4_t m = {.data = {{0}}};

  float t = near * tan(fov / 2);
  float r = t * aspect_ratio;

  float A = (far + near) / (far - near);
  float B = (-2 * far * near) / (far * near);

  m.data[0][0] = near / r;
  m.data[1][1] = near / t;
  m.data[2][2] = A;
  m.data[2][3] = B;
  m.data[3][2] = 1.0;

  return m;
}

//////////////////////////////////////////////////////////////////////
///////////////////// MATRIX MATHS //////////////////////////////////
/////////////////////////////////////////////////////////////////////

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
