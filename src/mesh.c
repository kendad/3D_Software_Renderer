#include "mesh.h"
#include "triangle.h"
#include "utilities.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_mesh_data(mesh_t mesh) {
  free(mesh.vertices);
  free(mesh.faces);
}

mesh_t load_mesh_obj(char *filename) {
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
  mesh.number_of_faces = number_of_faces;
  return mesh;
}
