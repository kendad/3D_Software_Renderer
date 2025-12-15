#include "appstate.h"
#include "camera.h"
#include "config.h"
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "utilities.h"
#include "vector.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_stdinc.h>
#include <SDL_timer.h>
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
    process_input(&app_state);

    // Mantain Frame rate here
    float time_to_wait = PER_FRAME_TARGET_TIME -
                         (SDL_GetTicks() - app_state.previous_frame_time);
    if (time_to_wait > 0) {
      SDL_Delay(time_to_wait);
    }
    // before updating the previous_frame_time calculate the delta time
    app_state.delta_time =
        (SDL_GetTicks() - app_state.previous_frame_time) / 1000.0; // in seconds
    // update the previous_frame_time
    app_state.previous_frame_time = SDL_GetTicks();

    update(&app_state);
    render(&app_state);
  }

  cleanup(&app_state);

  return 0;
}

mesh_t mesh;
int triangles_to_render_count = 0;
camera_t camera;
void setup(app_state_t *app_state) {
  // Set up the Frame Timer and the delta timer
  app_state->previous_frame_time = 0.0;
  app_state->delta_time = 0.0;

  //////////////////////////////////////////////////////////////////
  // load_cube_mesh_data();
  mesh = load_mesh_obj("../assets/sphere.obj");
  triangles_to_render = malloc(sizeof(triangle_t) * mesh.number_of_faces);

  // load the base camera
  camera = create_base_camera();

  display_init(app_state);

  // disable the visual of mouse cursor
  // SDL_ShowCursor(SDL_DISABLE);
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void process_input(app_state_t *app_state) {
  SDL_Event event;
  SDL_PollEvent(&event);

  // update mouse position deltas
  SDL_GetRelativeMouseState(&camera.delta_x_mouse_movement,
                            &camera.delta_y_mouse_movement);

  switch (event.type) {
  case SDL_QUIT:
    app_state->is_running = false;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE) {
      app_state->is_running = false;
    }
    if (event.key.keysym.sym == SDLK_w) {
      vec3_t forward_velocity = camera.direction;
      vec3_mul(&forward_velocity, 2.0 * app_state->delta_time);
      camera.position = vec3_add(camera.position, forward_velocity);
    }
    if (event.key.keysym.sym == SDLK_s) {
      vec3_t forward_velocity = camera.direction;
      vec3_mul(&forward_velocity, 2.0 * app_state->delta_time);
      camera.position = vec3_sub(camera.position, forward_velocity);
    }
    if (event.key.keysym.sym == SDLK_a) {
      camera.position.x -= 2.0 * app_state->delta_time;
    }
    if (event.key.keysym.sym == SDLK_d) {
      camera.position.x += 2.0 * app_state->delta_time;
    }
    break;
  }
}

// TRANSFORMATION INITIALIZER
color_t color_white = {0xFF, 0xFF, 0xFF};
float rotation_Y = 0.0;
float scale_Y = 1.0;
// CAMERA INITIALIZER
// UP vector for the camera
vec3_t camera_up_vector = {0, 1, 0};
// PROJECTION INITIALIZER
const float fov_vertical = M_PI / 3.0;
const float near = 0.1;
const float far = 1000.0;
const float aspect_ratio = (float)WINDOW_WIDTH / WINDOW_HEIGHT;

void update(app_state_t *app_state) {
  /////////////////////////////////////////////////////////////
  // reset triangles to render count
  triangles_to_render_count = 0;

  // Create a Rotation Matrix for rotation around Y-Axis
  rotation_Y += 0.5 * app_state->delta_time;
  mat4_t rotation_matrix_X = mat4_make_rotation_x(rotation_Y);
  mat4_t rotation_matrix_Y = mat4_make_rotation_y(rotation_Y);
  mat4_t rotation_matrix_Z = mat4_make_rotation_z(rotation_Y);

  // scale_Y += 0.001;
  mat4_t scale_matrix = mat4_make_scale(1.0, scale_Y, 1.0);

  // Create a Model to World Space Matrix
  mat4_t translation_matrix = mat4_make_translation(0, 0, 0);

  // Create a perspective matrix
  mat4_t perspective_matrix =
      mat4_make_perspective(fov_vertical, aspect_ratio, near, far);

  // Create a View Matrix
  vec4_t base_camera_direction = {0, 0, 1, 0};
  mat4_t camera_yaw_rotation = mat4_make_rotation_y(
      (float)camera.delta_x_mouse_movement * app_state->delta_time * 0.1);
  // apply yaw rotation to base camera direction to get the new direction
  vec4_t rotated_direction =
      mat4_mul_vec4(camera_yaw_rotation, base_camera_direction);
  camera.direction = vec3_from_vec4(rotated_direction);
  vec3_t camera_target = vec3_add(camera.position, camera.direction);

  mat4_t view_matrix =
      mat4_make_look_at(camera.position, camera_target, camera_up_vector);

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
      // transformed_points = mat4_mul_vec4(rotation_matrix_Y,
      // transformed_points); transformed_points =
      // mat4_mul_vec4(rotation_matrix_Z, transformed_points);

      // Translation
      transformed_points =
          mat4_mul_vec4(translation_matrix, transformed_points);

      triangle.vertices[j] = vec3_from_vec4(transformed_points);
    }

    // Move to View Space
    for (int i = 0; i < 3; ++i) {
      triangle.vertices[i] = vec3_from_vec4(
          mat4_mul_vec4(view_matrix, vec4_from_vec3(triangle.vertices[i])));
    }

    // Back face culling
    vec3_t vertex_a = triangle.vertices[0];
    vec3_t vertex_b = triangle.vertices[1];
    vec3_t vertex_c = triangle.vertices[2];

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

    // perspective divide/projecion
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

    for (int j = 0; j < 3; ++j) {
      // scale NDC[-1 to 1] to SCREEN_SPACE[0,1]
      triangle.vertices[j].x = (triangle.vertices[j].x + 1.0) * 0.5;
      triangle.vertices[j].y = (triangle.vertices[j].y + 1.0) * 0.5;

      // Scale the SCREEN_SPACE from[0,1] to [0,Width/Height]
      triangle.vertices[j].x *= WINDOW_WIDTH;
      triangle.vertices[j].y *= WINDOW_HEIGHT;
    }

    triangles_to_render[triangles_to_render_count++] = triangle;
  }
  /////////////////////////////////////////////////////////////
}

void render(app_state_t *app_state) {
  display_clear_buffer(app_state, 0xFF000000);
  ////////////////////////////////////////////////////////////
  for (int i = 0; i < triangles_to_render_count; ++i) {
    // draw_triangle_fill(triangles_to_render[i], app_state);
    draw_triangle_wireframe(triangles_to_render[i], app_state);
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
