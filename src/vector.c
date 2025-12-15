#include "vector.h"
#include <math.h>

///////////////////////////////////////////////////////
///////////////////// VECTOR 2D ///////////////////////
///////////////////////////////////////////////////////

vec2_t vec2_new(float x, float y) {
  vec2_t new_vector = {.x = x, .y = y};
  return new_vector;
}

float vec2_magnitude(vec2_t v) { return sqrt((v.x * v.x) + (v.y * v.y)); };

void vec2_mul(vec2_t *v, float factor) {
  v->x *= factor;
  v->y *= factor;
}
void vec2_div(vec2_t *v, float factor) {
  v->x /= factor;
  v->y /= factor;
}

vec2_t vec2_add(const vec2_t a, const vec2_t b) {
  vec2_t new_vector = {.x = a.x + b.x, .y = a.y + b.y};
  return new_vector;
}
vec2_t vec2_sub(const vec2_t a, const vec2_t b) {
  vec2_t new_vector = {.x = a.x - b.x, .y = a.y - b.y};
  return new_vector;
}

float vec2_cross(vec2_t a, vec2_t b) { return (a.x * b.y) - (b.x * a.y); }

///////////////////////////////////////////////////////
///////////////////// VECTOR 3D ///////////////////////
///////////////////////////////////////////////////////

vec3_t vec3_clone(vec3_t *v) {
  vec3_t new_vector = {.x = v->x, .y = v->y, .z = v->z};
  return new_vector;
}

vec3_t vec3_new(float x, float y, float z) {
  vec3_t new_vector = {.x = x, .y = y, .z = z};
  return new_vector;
}

float vec3_magnitude(vec3_t v) {
  return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}
void vec3_mul(vec3_t *v, float factor) {
  v->x *= factor;
  v->y *= factor;
  v->z *= factor;
}
void vec3_div(vec3_t *v, float factor) {
  v->x /= factor;
  v->y /= factor;
  v->z /= factor;
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
  vec3_t new_vector = {.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
  return new_vector;
}
vec3_t vec3_sub(vec3_t a, vec3_t b) {
  vec3_t new_vector = {.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
  return new_vector;
}

void vec3_normalize(vec3_t *v) {
  float magnitude = vec3_magnitude(*v);
  if (magnitude == 0)
    return;

  v->x /= magnitude;
  v->y /= magnitude;
  v->z /= magnitude;
}
float vec3_dot(vec3_t a, vec3_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
  vec3_t new_vector = {
      .x = (a.y * b.z) - (a.z * b.y),
      .y = (a.z * b.x) - (a.x * b.z),
      .z = (a.x * b.y) - (a.y * b.x),
  };
  return new_vector;
}

vec2_t vec2_from_vec3(vec3_t v) {
  vec2_t new_vector = {.x = v.x, .y = v.y};
  return new_vector;
}

///////////////////////////////////////////////////////
///////////////////// VECTOR 4D ///////////////////////
///////////////////////////////////////////////////////

vec4_t vec4_from_vec3(vec3_t v) {
  vec4_t new_vector = {.x = v.x, .y = v.y, .z = v.z, .w = 1.0};
  return new_vector;
}

vec3_t vec3_from_vec4(vec4_t v) {
  vec3_t new_vector = {.x = v.x, .y = v.y, .z = v.z};
  return new_vector;
}

vec2_t vec2_from_vec4(vec4_t v) {
  vec2_t new_vector = {.x = v.x, .y = v.y};
  return new_vector;
}
