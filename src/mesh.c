#include "mesh.h"
#include "triangle.h"
#include "utilities.h"
#include "vector.h"
#include <stdlib.h>

vec3_t cube_vertices[8] = {
    {.x = -1, .y = -1, .z = -1}, // 1
    {.x = -1, .y = 1, .z = -1},  // 2
    {.x = 1, .y = 1, .z = -1},   // 3
    {.x = 1, .y = -1, .z = -1},  // 4
    {.x = 1, .y = 1, .z = 1},    // 5
    {.x = 1, .y = -1, .z = 1},   // 6
    {.x = -1, .y = 1, .z = 1},   // 7
    {.x = -1, .y = -1, .z = 1}   // 8
};

face_t cube_faces[12] = {
    // front
    {.a = 1,
     .b = 2,
     .c = 3,
     .a_uv = {0, 1},
     .b_uv = {0, 0},
     .c_uv = {1, 0},
     .color = 0xFFFFFFFF},
    {.a = 1,
     .b = 3,
     .c = 4,
     .a_uv = {0, 1},
     .b_uv = {1, 0},
     .c_uv = {1, 1},
     .color = 0xFFFFFFFF},
    // right
    {.a = 4,
     .b = 3,
     .c = 5,
     .a_uv = {0, 1},
     .b_uv = {0, 0},
     .c_uv = {1, 0},
     .color = 0xFFFFFFFF},
    {.a = 4,
     .b = 5,
     .c = 6,
     .a_uv = {0, 1},
     .b_uv = {1, 0},
     .c_uv = {1, 1},
     .color = 0xFFFFFFFF},
    // back
    {.a = 6,
     .b = 5,
     .c = 7,
     .a_uv = {0, 1},
     .b_uv = {0, 0},
     .c_uv = {1, 0},
     .color = 0xFFFFFFFF},
    {.a = 6,
     .b = 7,
     .c = 8,
     .a_uv = {0, 1},
     .b_uv = {1, 0},
     .c_uv = {1, 1},
     .color = 0xFFFFFFFF},
    // left
    {.a = 8,
     .b = 7,
     .c = 2,
     .a_uv = {0, 1},
     .b_uv = {0, 0},
     .c_uv = {1, 0},
     .color = 0xFFFFFFFF},
    {.a = 8,
     .b = 2,
     .c = 1,
     .a_uv = {0, 1},
     .b_uv = {1, 0},
     .c_uv = {1, 1},
     .color = 0xFFFFFFFF},
    // top
    {.a = 2,
     .b = 7,
     .c = 5,
     .a_uv = {0, 1},
     .b_uv = {0, 0},
     .c_uv = {1, 0},
     .color = 0xFFFFFFFF},
    {.a = 2,
     .b = 5,
     .c = 3,
     .a_uv = {0, 1},
     .b_uv = {1, 0},
     .c_uv = {1, 1},
     .color = 0xFFFFFFFF},
    // bottom
    {.a = 6,
     .b = 8,
     .c = 1,
     .a_uv = {0, 1},
     .b_uv = {0, 0},
     .c_uv = {1, 0},
     .color = 0xFFFFFFFF},
    {.a = 6,
     .b = 1,
     .c = 4,
     .a_uv = {0, 1},
     .b_uv = {1, 0},
     .c_uv = {1, 1},
     .color = 0xFFFFFFFF}};

mesh_t mesh = {.vertices = NULL, .faces = NULL};
triangle_t cube_mesh_triangle_faces[12] = {0};

void load_cube_mesh_data(void) {
  mesh.vertices = malloc(sizeof(vec3_t) * 8);
  mesh.faces = malloc(sizeof(face_t) * 12);

  // load the vertices
  for (int i = 0; i < 8; ++i) {
    mesh.vertices[i] = cube_vertices[i];
  }
  // load the faces
  for (int i = 0; i < 12; ++i) {
    mesh.faces[i] = cube_faces[i];

    // load the triangle for the cube mesh faces
    color_t white_color = {0xFF, 0xFF, 0xFF};
    triangle_t triangle = {.vertices = {cube_vertices[mesh.faces[i].a - 1],
                                        cube_vertices[mesh.faces[i].b - 1],
                                        cube_vertices[mesh.faces[i].c - 1]},
                           .texcoords = {mesh.faces[i].a_uv, mesh.faces[i].b_uv,
                                         mesh.faces[i].c_uv},
                           .colors = {white_color, white_color, white_color}};
    cube_mesh_triangle_faces[i] = triangle;
  }
}

void free_mesh_data(mesh_t mesh) {
  free(mesh.vertices);
  free(mesh.faces);
}
