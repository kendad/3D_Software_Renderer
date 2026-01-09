#include "mesh.h"
#include "clipping.h"
#include "config.h"
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

void mesh_apply_transform_view_projection(
    mesh_t *mesh, triangle_t *triangles_to_render,
    int *triangles_to_render_count, mat4_t scale_matrix, mat4_t rotation_matrix,
    mat4_t translation_matrix, mat4_t view_matrix, mat4_t projection_matrix,
    bool is_skybox) {

  // loop through all the faces/triangles
  for (int i = 0; i < mesh->number_of_faces; ++i) {
    triangle_t triangle;
    // One face is one triangle
    triangle.vertices[0] = vec4_from_vec3(mesh->vertices[mesh->faces[i].a]);
    triangle.vertices[1] = vec4_from_vec3(mesh->vertices[mesh->faces[i].b]);
    triangle.vertices[2] = vec4_from_vec3(mesh->vertices[mesh->faces[i].c]);
    triangle.normals[0] = mesh->normals[mesh->faces[i].n_a];
    triangle.normals[1] = mesh->normals[mesh->faces[i].n_b];
    triangle.normals[2] = mesh->normals[mesh->faces[i].n_c];
    triangle.texcoords[0] = mesh->tex_coords[mesh->faces[i].a_uv];
    triangle.texcoords[1] = mesh->tex_coords[mesh->faces[i].b_uv];
    triangle.texcoords[2] = mesh->tex_coords[mesh->faces[i].c_uv];

    // Transformations
    for (int j = 0; j < 3; ++j) {
      vec4_t transformed_points = triangle.vertices[j];
      vec4_t transformed_normals = vec4_from_vec3(triangle.normals[j]);

      // Scale
      transformed_points = mat4_mul_vec4(scale_matrix, transformed_points);

      // Rotations
      transformed_points = mat4_mul_vec4(rotation_matrix, transformed_points);

      // Also Rotate the normals
      transformed_normals = mat4_mul_vec4(rotation_matrix, transformed_normals);

      // Translation
      transformed_points =
          mat4_mul_vec4(translation_matrix, transformed_points);

      triangle.vertices[j] = transformed_points;
      triangle.normals[j] = vec3_from_vec4(transformed_normals);
    }

    // Move the vertices and normals to View Space
    for (int j = 0; j < 3; ++j) {
      triangle.vertices[j] = mat4_mul_vec4(view_matrix, triangle.vertices[j]);

      // store the view space vertices that will be further used for lighting
      // calculations
      triangle.view_space_vertices[j] = triangle.vertices[j];

      vec4_t normal = vec4_from_vec3(triangle.normals[j]);
      normal.w = 0.0; // this removes translation from the normal as we dont
                      // want to move normals only rotate them
      triangle.normals[j] = vec3_from_vec4(mat4_mul_vec4(view_matrix, normal));
      vec3_normalize(&triangle.normals[j]);
    }

    // Back face culling
    vec3_t vertex_a = vec3_from_vec4(triangle.vertices[0]);
    vec3_t vertex_b = vec3_from_vec4(triangle.vertices[1]);
    vec3_t vertex_c = vec3_from_vec4(triangle.vertices[2]);

    vec3_t ab = vec3_sub(vertex_b, vertex_a);
    vec3_t ac = vec3_sub(vertex_c, vertex_a);
    vec3_t normal = vec3_cross(ac, ab);
    vec3_normalize(&normal);

    vec3_t origin = {0.0, 0.0, 0.0};
    vec3_t camera_ray = vec3_sub(origin, vertex_a);
    vec3_normalize(&camera_ray);

    float dot = vec3_dot(normal, camera_ray);
    if (dot < 0) {
      continue;
    }

    // perspective projecion->perspective divide
    for (int j = 0; j < 3; ++j) {
      // perspective projection
      vec4_t projected_points = triangle.vertices[j];
      projected_points = mat4_mul_vec4(projection_matrix, projected_points);
      if (is_skybox) {
        projected_points.z = 0.0001;
      }
      triangle.vertices[j] = projected_points;
    }

    // CLIPPING Space
    polygon_t polygon = create_polygon_from_triangle(triangle);
    clip_polygon(&polygon);
    // after clipping we get new set of vertices which we will need to create
    // new triangles
    triangle_t triangles_after_clipping[MAX_NUM_POLYGON_VERTICES];
    int num_triangles_after_clipping;
    triangle_from_polygon(&polygon, triangles_after_clipping,
                          &num_triangles_after_clipping);

    // loop through this new set of triangles
    for (int ct = 0; ct < num_triangles_after_clipping;
         ++ct) { // ct->clipped triangle
      triangle = triangles_after_clipping[ct];
      // perspective divide
      for (int j = 0; j < 3; ++j) {
        // Will also scale the values in the range [-1,1]
        triangle.vertices[j].x /= triangle.vertices[j].w;
        triangle.vertices[j].y /= triangle.vertices[j].w;
        triangle.vertices[j].z /= triangle.vertices[j].w;
      }

      for (int j = 0; j < 3; ++j) {
        // scale NDC[-1 to 1] to SCREEN_SPACE[0,1]
        triangle.vertices[j].x = (triangle.vertices[j].x + 1.0) * 0.5;
        triangle.vertices[j].y = (triangle.vertices[j].y + 1.0) * 0.5;

        // Scale the SCREEN_SPACE from[0,1] to [0,Width/Height]
        triangle.vertices[j].x *= WINDOW_WIDTH;
        triangle.vertices[j].y *= WINDOW_HEIGHT;
      }

      triangles_to_render[*triangles_to_render_count] = triangle;
      (*triangles_to_render_count)++;
    }
  }
}
