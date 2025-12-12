#pragma once

typedef struct {
  float x, y;
} vec2_t;

typedef struct {
  float x, y, z;
} vec3_t;

typedef struct {
  float x, y, z, w;
} vec4_t;

///////////////////////////////////////////////////////
///////////////////// VECTOR 2D ///////////////////////
///////////////////////////////////////////////////////
vec2_t vec2_new(float x, float y);
float vec2_magnitude(vec2_t v);
vec2_t vec2_add(const vec2_t a, const vec2_t b);
vec2_t vec2_sub(const vec2_t a, const vec2_t b);
void vec2_mul(vec2_t *v, float factor);
void vec2_div(vec2_t *v, float factor);

float vec2_cross(vec2_t a, vec2_t b);

///////////////////////////////////////////////////////
///////////////////// VECTOR 3D ///////////////////////
///////////////////////////////////////////////////////

vec3_t vec3_clone(vec3_t *v);
vec3_t vec3_new(float x, float y, float z);
float vec3_magnitude(vec3_t v);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
void vec3_mul(vec3_t *v, float factor);
void vec3_div(vec3_t *v, float factor);

void vec3_normalize(vec3_t *v);
float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);

vec2_t vec2_from_vec3(vec3_t v);

///////////////////////////////////////////////////////
///////////////////// VECTOR 4D ///////////////////////
///////////////////////////////////////////////////////

vec4_t vec4_from_vec3(vec3_t v);
vec3_t vec3_from_vec4(vec4_t v);
vec2_t vec2_from_vec4(vec4_t v);
