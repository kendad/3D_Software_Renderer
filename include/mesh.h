#pragma once
#include "texture.h"
#include "triangle.h"
#include "vector.h"
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
