#include "mesh.h"
#include "stb_image.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_mesh_data(mesh_t mesh) {
  free(mesh.vertices);
  free(mesh.normals);
  free(mesh.tex_coords);
  free(mesh.faces);
  // free(mesh.texture_data.data);
  stbi_image_free(mesh.texture_data.data);
}

mesh_t load_mesh_obj(char *obj_filename, char *texture_filename) {
  mesh_t mesh = {0};

  FILE *file_pointer = fopen(obj_filename, "r");
  if (!file_pointer) {
    fprintf(stderr, "Error: Could nor open file: %s\n", obj_filename);
    exit(1);
  }

  char line[1024];

  // loop through the OBJ file line by line
  // to get the number of vertices and faces
  int number_of_vertices = 0;
  int number_of_normals = 0;
  int number_of_texcoords = 0;
  int number_of_faces = 0;
  while (fgets(line, sizeof(line), file_pointer)) {
    // Parse the vertex line
    if (strncmp(line, "v ", 2) == 0) {
      number_of_vertices++;
    }
    // Parse the Normal line
    if (strncmp(line, "vn ", 3) == 0) {
      number_of_normals++;
    }
    // Parse the texture coordinate line
    if (strncmp(line, "vt ", 3) == 0) {
      number_of_texcoords++;
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
  mesh.normals = malloc(sizeof(vec3_t) * number_of_normals);
  int current_normal = 0;
  mesh.tex_coords = malloc(sizeof(tex2_t) * number_of_texcoords);
  int current_texcoord = 0;
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

    // Parse the normal line
    if (strncmp(line, "vn ", 3) == 0) {
      vec3_t normal;
      sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
      // normal.y *= -1;
      mesh.normals[current_normal++] = normal;
    }

    // Parse the vertex coordinate line
    if (strncmp(line, "vt ", 3) == 0) {
      tex2_t texcoord;
      sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
      texcoord.v = 1.0 - texcoord.v;
      mesh.tex_coords[current_texcoord++] = texcoord;
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
                     .n_a = normal_indices[0] - 1,
                     .n_b = normal_indices[1] - 1,
                     .n_c = normal_indices[2] - 1,
                     .a_uv = texture_indices[0] - 1,
                     .b_uv = texture_indices[1] - 1,
                     .c_uv = texture_indices[2] - 1,
                     .color = 0xFFFFFFFF};
      mesh.faces[current_face++] = face;
    }
  }
  mesh.number_of_faces = number_of_faces;

  // load the texture data
  mesh.texture_data = load_texture_data(texture_filename);
  return mesh;
}
