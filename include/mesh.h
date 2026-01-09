#pragma once
#include "matrix.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  vec3_t *vertices;
  vec3_t *normals;
  tex2_t *tex_coords;
  face_t *faces;
  texture_t texture_data;
  int number_of_faces;
} mesh_t;

/////////////////////////////////////////////////////
////////////////// EXAMPLE CUBE ////////////////////

void load_cube_mesh_data(void);
void free_mesh_data(mesh_t mesh);

///////////////////////////////////////////////////////
//////////////////////////////////////////////////////

mesh_t load_mesh_obj(char *obj_filename, char *texture_filename);

///////////////////////////////////////////////////////
//////////////////////////////////////////////////////

void mesh_apply_transform_view_projection(
    mesh_t *mesh, triangle_t *triangles_to_render,
    int *triangles_to_render_count, mat4_t scale_matrix, mat4_t rotation_matrix,
    mat4_t translation_matrix, mat4_t view_matrix, mat4_t projection_matrix,
    bool is_skybox);

// TO_DO: optimize mesh render fucntion for different lighting systems
void mesh_render_with_phong_lighting_model(triangle_t *triangles_to_render,
                                           int *triangles_to_render_count);
