#pragma once
#include "appstate.h"
#include "lights.h"
#include "texture.h"
#include "utilities.h"
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>

//                  'a'
//               vertices[0]
//               texcoords[0]
//               normals[0]
//                   a_uv
//                   /\
//                  /  \
//                 /    \
//                /______\
//              'b'      'c'
//        vertices[1]    vertices[2]
//        texcoords[1]   texcoords[2]
//        normals[1]     normals[2]
//            b_uv           c_uv

typedef struct {
  vec4_t vertices[3];
  vec4_t view_space_vertices[3];
  vec3_t normals[3];
  tex2_t texcoords[3];

  color_t colors[3];
} triangle_t;

typedef struct {
  int a;
  int b;
  int c;

  int n_a;
  int n_b;
  int n_c;

  int a_uv;
  int b_uv;
  int c_uv;

  uint32_t color;
} face_t;

typedef struct {
  int x_min;
  int y_min;
  int x_max;
  int y_max;
} bounding_box_t;

typedef struct {
  triangle_t *triangles_to_render;
  int *triangles_to_render_count;
  // BRDF related textures
  texture_t *base_texture_data;
  texture_t *radiance_texture_data;
  texture_t *irradiance_texture_data;
  texture_t *LUT_texture_data;
  bool is_PBR;
} material_t;

void draw_triangle_fill_with_lighting_effect(triangle_t triangle,
                                             material_t *material_data,
                                             scene_info_t *scene_info_t,
                                             app_state_t *app_state);

// To be used in multi threads
// where the texture is divided into a 32x32 tile space
void draw_triangle_fill_tiled_with_lighting_effect(
    triangle_t triangle, material_t *material_data, scene_info_t *scene_info,
    bounding_box_t tile_bounding_box, app_state_t *app_state);

void draw_triangle_wireframe(triangle_t triangle, app_state_t *app_state);
