#pragma once
#include "triangle.h"
#include "vector.h"

typedef struct {
  vec3_t *vertices;
  face_t *faces;
} mesh_t;

/////////////////////////////////////////////////////
////////////////// EXAMPLE CUBE ////////////////////

// 8 vertices of a CUBE
extern vec3_t cube_vertices[8];

// 6 faces of a cube and each one made of two triangles
extern face_t cube_faces[12];

extern mesh_t mesh;
extern triangle_t cube_mesh_triangle_faces[12];

void load_cube_mesh_data(void);
void free_mesh_data(mesh_t mesh);
///////////////////////////////////////////////////////
//////////////////////////////////////////////////////
