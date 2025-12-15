#include "appstate.h"
#include "config.h"
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "utilities.h"
#include "vector.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void setup(app_state_t *app_state);
void process_input(app_state_t *app_state);
void update(app_state_t *app_state);
void render(app_state_t *app_state);
void cleanup(app_state_t *app_state);

//////////////////////////////////////////////////////////////
triangle_t *triangles_to_render;
///////////////////////////////////////////////////////////////

int main(void) {
  app_state_t app_state = {0};

  // intialize
  setup(&app_state);

  while (app_state.is_running) {
    update(&app_state);
    render(&app_state);
  }

  cleanup(&app_state);

  return 0;
}

mesh_t mesh;
void setup(app_state_t *app_state) {

  //////////////////////////////////////////////////////////////////
  // load_cube_mesh_data();
  mesh = load_mesh_obj("../assets/f22.obj");
  triangles_to_render = malloc(sizeof(triangle_t) * mesh.number_of_faces);
  //////////////////////////////////////////////////////////////////
  display_init(app_state);
}

void process_input(app_state_t *app_state) {}

color_t color_white = {0xFF, 0xFF, 0xFF};
float rotation_Y = 0.0;
float scale_Y = 1.0;
const float fov_vertical = M_PI / 3.0;
const float near = 0.1;
const float far = 1000.0;
const float aspect_ratio = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
void update(app_state_t *app_state) {
  /////////////////////////////////////////////////////////////

  // Create a Rotation Matrix for rotation around Y-Axis
  rotation_Y += 0.001;
  mat4_t rotation_matrix_X = mat4_make_rotation_x(rotation_Y);
  mat4_t rotation_matrix_Y = mat4_make_rotation_y(rotation_Y);
  mat4_t rotation_matrix_Z = mat4_make_rotation_z(rotation_Y);

  // scale_Y += 0.001;
  mat4_t scale_matrix = mat4_make_scale(1.0, scale_Y, 1.0);

  mat4_t translation_matrix = mat4_make_translation(0, 0, 0);

  // Applying Simple projection here
  for (int i = 0; i < mesh.number_of_faces; ++i) {
    triangle_t triangle;
    // One face is one triangle
    triangle.vertices[0] = mesh.vertices[mesh.faces[i].a];
    triangle.vertices[1] = mesh.vertices[mesh.faces[i].b];
    triangle.vertices[2] = mesh.vertices[mesh.faces[i].c];
    triangle.colors[0] = color_white;
    triangle.colors[1] = color_white;
    triangle.colors[2] = color_white;

    // Transformations
    for (int j = 0; j < 3; ++j) {
      vec4_t transformed_points = vec4_from_vec3(triangle.vertices[j]);
      // Scale
      transformed_points = mat4_mul_vec4(scale_matrix, transformed_points);

      // Rotations
      transformed_points = mat4_mul_vec4(rotation_matrix_X, transformed_points);
      transformed_points = mat4_mul_vec4(rotation_matrix_Y, transformed_points);
      transformed_points = mat4_mul_vec4(rotation_matrix_Z, transformed_points);

      // Translation
      transformed_points =
          mat4_mul_vec4(translation_matrix, transformed_points);

      triangle.vertices[j] = vec3_from_vec4(transformed_points);
      triangle.vertices[j].z += 5.0; // move the mesh towards the forward z-axis
    }

    // perspective divide/projecion
    // Create a perspective matrix
    mat4_t perspective_matrix =
        mat4_make_perspective(fov_vertical, aspect_ratio, near, far);
    for (int j = 0; j < 3; ++j) {
      vec4_t projected_points = vec4_from_vec3(triangle.vertices[j]);
      projected_points = mat4_mul_vec4(
          perspective_matrix,
          projected_points); // Brings the values into the -1 and 1 range
      // perspective divide
      projected_points.x /= projected_points.w;
      projected_points.y /= projected_points.w;
      projected_points.z /= projected_points.w;
      projected_points.w /= projected_points.w;

      triangle.vertices[j] = vec3_from_vec4(projected_points);
    }
    // scale the coordinates from NDC[-1,1] to ScreenSpace[0,Width]and[0,Height]
    for (int j = 0; j < 3; ++j) {
      triangle.vertices[j].x *= (WINDOW_WIDTH / 2.0);
      triangle.vertices[j].y *= (WINDOW_HEIGHT / 2.0);
    }
    // translate the points to [0 to WIDHT/HEIGHT] range (SCREEN SPACE
    // COORDINATES)
    for (int j = 0; j < 3; ++j) {
      triangle.vertices[j].x += (WINDOW_WIDTH / 2.0);
      triangle.vertices[j].y += (WINDOW_HEIGHT / 2.0);
    }
    triangles_to_render[i] = triangle;
  }
  /////////////////////////////////////////////////////////////
}

void render(app_state_t *app_state) {
  display_clear_buffer(app_state, 0xFF000000);

  ////////////////////////////////////////////////////////////
  for (int i = 0; i < mesh.number_of_faces; ++i) {
    fill_triangle(triangles_to_render[i], app_state);
  }
  /////////////////////////////////////////
  //////////////////////

  display_render_buffer(app_state);
}

void cleanup(app_state_t *app_state) {
  free(triangles_to_render);
  free_mesh_data(mesh);
  display_cleanup(app_state);
}
