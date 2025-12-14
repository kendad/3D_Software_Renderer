#include "mesh.h"
#include "triangle.h"
#include "utilities.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    // load the triangle for the cube mesh facrotation_Yes
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

void load_mesh_obj(char *filename) {
  mesh_t mesh = {0};

  FILE *file_pointer = fopen(filename, "r");
  if (!file_pointer) {
    fprintf(stderr, "Error: Could nor open file: %s\n", filename);
    exit(1);
  }

  char line[1024];

  // loop through the OBJ file line by line
  // to get the number of vertices and faces
  int number_of_vertices = 0;
  int number_of_faces = 0;
  while (fgets(line, sizeof(line), file_pointer)) {
    // Parse the vertex line
    if (strncmp(line, "v ", 2) == 0) {
      number_of_vertices++;
    }
    // Parse the face line
    if (strncmp(line, "f ", 2) == 0) {
      number_of_faces++;
    }
  }

  // loop through the OBJ file second time to allocate required
  // memory space as well as the actual data
  mesh.vertices = malloc(sizeof(vec3_t) * number_of_vertices);
  int current_vertex = 0;
  mesh.faces = malloc(sizeof(face_t) * number_of_faces);
  int current_face = 0;

  // Reset the file pointer to the start of the OBJ text file
  if (fseek(file_pointer, 0, SEEK_SET) != 0) {
    fprintf(stderr, "ERROR:Failed to reset file pointer\n");
    exit(1);
  }

  while (fgets(line, sizeof(line), file_pointer)) {
    // Parse the vertex line
    if (strncmp(line, "v ", 2) == 0) {
      vec3_t vertex;
      sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
      vertex.y *= -1;
      mesh.vertices[current_vertex++] = vertex;
    }
    // Parse the face line
    if (strncmp(line, "f ", 2) == 0) {
      int vertex_indices[3];
      int texture_indices[3];
      int normal_indices[3];
      sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_indices[0],
             &texture_indices[0], &normal_indices[0], &vertex_indices[1],
             &texture_indices[1], &normal_indices[1], &vertex_indices[2],
             &texture_indices[2], &normal_indices[2]);
      face_t face = {.a = vertex_indices[0] - 1,
                     .b = vertex_indices[1] - 1,
                     .c = vertex_indices[2] - 1,
                     .color = 0xFFFFFFFF};
      mesh.faces[current_face++] = face;
    }
  }
  // For now just load the mesh data into the cube mesh triangle array
  color_t white_color = {0xFF, 0xFF, 0xFF};
  for (int i = 0; i < 12; ++i) {
    triangle_t triangle = {.vertices = {mesh.vertices[mesh.faces[i].a],
                                        mesh.vertices[mesh.faces[i].b],
                                        mesh.vertices[mesh.faces[i].c]},
                           .colors = {white_color, white_color, white_color}};
    cube_mesh_triangle_faces[i] = triangle;
  }
}
