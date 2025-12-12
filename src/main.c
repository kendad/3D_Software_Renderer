#include "appstate.h"
#include "config.h"
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

void setup(app_state_t *app_state);
void process_input(app_state_t *app_state);
void update(app_state_t *app_state);
void render(app_state_t *app_state);
void cleanup(app_state_t *app_state);

//////////////////////////////////////////////////////////////
triangle_t triangles_to_render[12];
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

void setup(app_state_t *app_state) {

  //////////////////////////////////////////////////////////////////
  load_cube_mesh_data();
  //////////////////////////////////////////////////////////////////
  display_init(app_state);
}

void process_input(app_state_t *app_state) {}

float rotation_Y = 0.0;
const float fov_vertical = M_PI / 3.0;
const float near = 0.1;
const float far = 1000.0;
const float aspect_ratio = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
void update(app_state_t *app_state) {
  /////////////////////////////////////////////////////////////

  // Create a Rotation Matrix for rotation around Y-Axis
  rotation_Y += 0.001;
  mat4_t rotation_matrix_Y = mat4_make_rotation_y(rotation_Y);

  // Applying Simple projection here
  for (int i = 0; i < 12; ++i) {
    triangle_t triangle = cube_mesh_triangle_faces[i];
    // Transformations
    for (int j = 0; j < 3; ++j) {
      // Rotations
      vec4_t transformed_points = vec4_from_vec3(triangle.vertices[j]);
      transformed_points = mat4_mul_vec4(rotation_matrix_Y, transformed_points);
      triangle.vertices[j] = vec3_from_vec4(transformed_points);

      // Translation
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
  for (int i = 0; i < 12; ++i) {
    fill_triangle(triangles_to_render[i], app_state);
  }
  // fill_triangle(sample_triangle, app_state);
  /////////////////////////////////////////
  //////////////////////

  display_render_buffer(app_state);
}

void cleanup(app_state_t *app_state) {
  free_mesh_data(mesh);
  display_cleanup(app_state);
}
