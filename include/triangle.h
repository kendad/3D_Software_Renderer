#pragma once
#include "appstate.h"
#include "texture.h"
#include "utilities.h"
#include "vector.h"
#include <stdint.h>

//                  'a'
//               vertices[0]
//               texcoords[0]
//                   a_uv
//                   /\
//                  /  \
//                 /    \
//                /______\
//              'b'      'c'
//        vertices[1]    vertices[2]
//        texcoords[1]   texcoords[2]
//            b_uv           c_uv

typedef struct {
  vec4_t vertices[3];
  tex2_t texcoords[3];

  color_t colors[3];
} triangle_t;

typedef struct {
  int a;
  int b;
  int c;

  int a_uv;
  int b_uv;
  int c_uv;

  uint32_t color;
} face_t;

void draw_triangle_fill(triangle_t triangle, texture_t *texture_data,
                        app_state_t *app_state);
void draw_triangle_wireframe(triangle_t triangle, app_state_t *app_state);
