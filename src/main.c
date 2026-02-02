#include "appstate.h"
#include "camera.h"
#include "config.h"
#include "display.h"
#include "lights.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "threads.h"
#include "triangle.h"
#include "utilities.h"
#include "vector.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_stdinc.h>
#include <SDL_timer.h>
#include <bits/pthreadtypes.h>
#include <math.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

void setup(app_state_t *app_state);
void process_input(app_state_t *app_state);
void update(app_state_t *app_state);
void render(app_state_t *app_state);
void render_with_threads(app_state_t *app_state);
void cleanup(app_state_t *app_state);

//////////////////////////////////////////////////////////////
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
    // render(&app_state); // run on single core
    render_with_threads(&app_state); // run on mutiple cores based on the
    // system availability
  }

  cleanup(&app_state);

  return 0;
}
/////////////////////////  INITIALIZERS
/////////////////////////////////////////////////////////////////////////////////////

// THREADS are declared here
pthread_t *thread_pool;  // an array of thread with an array size == no of cores
                         // in the system
thread_t *thread_data;   // data to be passed to each thread in the pool
atomic_int tile_counter; // a global storage of all the tile ID's
sem_t *start_signals;    // array of semaphore to signal to start the
                         // work[decerements a value ifnot zero]
sem_t *done_signals; // array of semaphore to signal that a thread has done its
                     // work[incrementsthe value]
bool is_main_thread_running;

// 3D Mesh
mesh_t mesh;
triangle_t *triangles_to_render;
int triangles_to_render_count = 0;
// SkyBox
mesh_t skybox;
triangle_t *triangles_to_render_in_skybox;
int triangles_to_render_in_skybox_count = 0;
// Radiance Cubemap Mesh
mesh_t radiance_cubemap_mesh;
// Irradiance Cubemap Mesh
mesh_t irradiance_cubemap_mesh;
// LUT texture data
texture_t LUT_texture_data;
// Base material
material_t base_material;
// skybox material
material_t skybox_material;

// Lights
light_t lights[MAX_NUMBER_OF_LIGHTS];
light_t view_space_lights[MAX_NUMBER_OF_LIGHTS];
int total_lights_in_scene = 0;

//  TRANSFORMATION INITIALIZER
color_t color_white = {0xFF, 0xFF, 0xFF};
float rotation_Y = 0.0;
float scale_Y = 1.0;

// CAMERA INITIALIZER
camera_t camera;
// UP vector for the camera
vec3_t camera_position_at_view_space = {0, 0, 0};
vec3_t camera_up_vector = {0, 1, 0};

// PROJECTION INITIALIZER
const float fov_vertical = M_PI / 3.0;
const float near = 0.01;
const float far = 1000.0;
const float aspect_ratio = (float)WINDOW_WIDTH / WINDOW_HEIGHT;

// Scene Info Initializer
scene_info_t scene_info;
//////////////////////////////////////////////////////////////////////////////////////

void setup(app_state_t *app_state) {
  // Set up the Frame Timer and the delta timer
  app_state->previous_frame_time = 0.0;
  app_state->delta_time = 0.0;

  //////////////////////////////////////////////////////////////////
  // load_cube_mesh_data();
  mesh = load_mesh_obj("../assets/register.obj", "../assets/register.png");
  triangles_to_render = malloc(sizeof(triangle_t) * mesh.number_of_faces);

  // load the skybox
  skybox = load_mesh_obj("../assets/skybox.obj", "../assets/club_cubemap.png");
  triangles_to_render_in_skybox =
      malloc(sizeof(triangle_t) * skybox.number_of_faces);

  // Load the LUT texture data
  LUT_texture_data = load_texture_data("../assets/IBL/club_r/LUT.png");

  // Load the Radiance Map
  // Load it based on the roughness level
  // extremely metal  then rougness level is 0
  // and the other way around will be a blurred at level[what ever max level you
  // have]
  radiance_cubemap_mesh =
      load_mesh_obj("../assets/skybox.obj",
                    "../assets/IBL/club_r/club_radiance_map_level_0.png");

  // Load the Irradiance Map
  irradiance_cubemap_mesh =
      load_mesh_obj("../assets/skybox.obj",
                    "../assets/IBL/club_ir/club_irradiance_cubemap.png");

  // Update the base material with the texture and triangle information
  base_material.triangles_to_render = triangles_to_render;
  base_material.triangles_to_render_count = &triangles_to_render_count;
  base_material.base_texture_data = &mesh.texture_data;
  base_material.radiance_texture_data = &radiance_cubemap_mesh.texture_data;
  base_material.irradiance_texture_data = &irradiance_cubemap_mesh.texture_data;
  base_material.LUT_texture_data = &LUT_texture_data;
  base_material.is_PBR = true;

  // Do the same for the skybox material
  skybox_material.triangles_to_render = triangles_to_render_in_skybox;
  skybox_material.triangles_to_render_count =
      &triangles_to_render_in_skybox_count;
  skybox_material.base_texture_data = &skybox.texture_data;
  skybox_material.radiance_texture_data = NULL;
  skybox_material.irradiance_texture_data = NULL;
  skybox_material.LUT_texture_data = NULL;
  skybox_material.is_PBR = false;

  // load the lights in the scene
  init_lights_in_scene(lights, &total_lights_in_scene);

  // load the base camera
  camera = create_base_camera();

  display_init(app_state);

  // disable the visual of mouse cursor
  SDL_SetRelativeMouseMode(SDL_TRUE);

  // intialize scene information
  scene_info.lights = lights;
  scene_info.total_lights_in_scene = &total_lights_in_scene;
  scene_info.camera_position = &camera_position_at_view_space;

  // initialize and start the threads
  is_main_thread_running = true;
  threads_initialize(app_state, &thread_pool, &thread_data, &start_signals,
                     &done_signals, &tile_counter, &is_main_thread_running,
                     &base_material, &skybox_material, &scene_info);
}

void process_input(app_state_t *app_state) {
  SDL_Event event;

  // Vectors for camera movements
  vec3_t forward_vector = camera.direction;
  vec3_t right_vector = vec3_cross(camera.direction, camera_up_vector);
  vec3_normalize(&right_vector);

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      app_state->is_running = false;
      break;
    case SDL_MOUSEMOTION: {
      int x_offset = event.motion.xrel;
      int y_offset = event.motion.yrel;
      camera.yaw += x_offset * camera.mouse_sensitivity;
      camera.pitch -= y_offset * camera.mouse_sensitivity;
      break;
    }
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        app_state->is_running = false;
      }
      // W-A-S-D movements
      if (event.key.keysym.sym == SDLK_w) {
        vec3_mul(&forward_vector, camera.speed * app_state->delta_time);
        camera.position = vec3_add(camera.position, forward_vector);
      }
      if (event.key.keysym.sym == SDLK_s) {
        vec3_mul(&forward_vector, camera.speed * app_state->delta_time);
        camera.position = vec3_sub(camera.position, forward_vector);
      }
      if (event.key.keysym.sym == SDLK_a) {
        vec3_mul(&right_vector, camera.speed * app_state->delta_time);
        camera.position = vec3_add(camera.position, right_vector);
      }
      if (event.key.keysym.sym == SDLK_d) {
        vec3_mul(&right_vector, camera.speed * app_state->delta_time);
        camera.position = vec3_sub(camera.position, right_vector);
        break;
      }
    }
  }
}

void update(app_state_t *app_state) {
  /////////////////////////////////////////////////////////////
  // reset triangles to render count
  triangles_to_render_count = 0;
  triangles_to_render_in_skybox_count = 0;

  // Create a Rotation Matrix for rotation around Y-Axis
  rotation_Y += 0.5 * app_state->delta_time;
  mat4_t rotation_matrix_X = mat4_make_rotation_x(rotation_Y);
  mat4_t rotation_matrix_Y = mat4_make_rotation_y(rotation_Y);
  mat4_t rotation_matrix_Z = mat4_make_rotation_z(rotation_Y);
  mat4_t rotation_matrix = mat4_make_identity();
  // rotation_matrix = mat4_mul_mat4(rotation_matrix, rotation_matrix_Z);
  rotation_matrix = mat4_mul_mat4(rotation_matrix, rotation_matrix_Y);
  // rotation_matrix = mat4_mul_mat4(rotation_matrix, rotation_matrix_X);

  mat4_t rotation_matrix_for_camera = mat4_make_identity();

  // scale_Y += 0.001;
  mat4_t scale_matrix = mat4_make_scale(1.0, scale_Y, 1.0);
  mat4_t scale_matrix_for_camera = mat4_make_scale(500.0, 500, 500.0);

  // Create a Model to World Space Matrix
  mat4_t translation_matrix = mat4_make_translation(0, 0, 0);
  mat4_t translation_matrix_to_camera_position = mat4_make_translation(
      camera.position.x, camera.position.y, camera.position.z);

  // Create a perspective matrix
  mat4_t perspective_matrix =
      mat4_make_perspective(fov_vertical, aspect_ratio, near, far);

  // Create a View Matrix based of on FPS CAMERA
  vec3_t camera_target = {0, 0, 1};
  mat4_t camera_yaw_matrix = mat4_make_rotation_y(camera.yaw);
  mat4_t camera_pitch_matrix = mat4_make_rotation_x(camera.pitch);
  mat4_t camera_rotation_matrix =
      mat4_mul_mat4(camera_yaw_matrix, camera_pitch_matrix);
  camera.direction = vec3_from_vec4(
      mat4_mul_vec4(camera_rotation_matrix, vec4_from_vec3(camera_target)));
  vec3_normalize(&camera.direction);
  camera_target = vec3_add(camera.position, camera.direction);

  mat4_t view_matrix =
      mat4_make_look_at(camera.position, camera_target, camera_up_vector);

  // move the lights into View Space
  for (int l = 0; l < total_lights_in_scene; ++l) {
    vec4_t light_pos = vec4_from_vec3(lights[l].position);
    view_space_lights[l].position =
        vec3_from_vec4(mat4_mul_vec4(view_matrix, light_pos));
    view_space_lights[l].color = lights[l].color;
  }

  // loop through all the faces/triangles
  mesh_apply_transform_view_projection(&mesh, triangles_to_render,
                                       &triangles_to_render_count, scale_matrix,
                                       rotation_matrix, translation_matrix,
                                       view_matrix, perspective_matrix, false);
  ///////////////////////////////////////////////////////////////////////////////
  mesh_apply_transform_view_projection(
      &skybox, triangles_to_render_in_skybox,
      &triangles_to_render_in_skybox_count, scale_matrix_for_camera,
      rotation_matrix_for_camera, translation_matrix_to_camera_position,
      view_matrix, perspective_matrix, true);
  //////////////////////////////////////////////////////////////////////////////
}

void render(app_state_t *app_state) {
  display_clear_buffer(app_state, 0xFF000000);
  display_clear_depth_buffer(app_state);
  ////////////////////////////////////////////////////////////
  //////////// Draw the Mesh /////////////////////////////////
  ////////////////////////////////////////////////////////////
  for (int i = 0; i < triangles_to_render_count; ++i) {
    draw_triangle_fill_with_lighting_effect(
        triangles_to_render[i], &base_material, &scene_info, app_state);
    // draw_triangle_wireframe(triangles_to_render[i], app_state);
  }

  ////////////////////////////////////////////////////////////
  //////////// Draw the SkyBox /////////////////////////////////
  ////////////////////////////////////////////////////////////

  for (int i = 0; i < triangles_to_render_in_skybox_count; ++i) {
    draw_triangle_fill_with_lighting_effect(triangles_to_render_in_skybox[i],
                                            &skybox_material, &scene_info,
                                            app_state);
  }
  /////////////////////////////////////////
  //////////////////////

  display_render_buffer(app_state);
}

void render_with_threads(app_state_t *app_state) {
  display_clear_buffer(app_state, 0xFF000000);
  display_clear_depth_buffer(app_state);
  int total_no_of_cores_in_the_system = sysconf(_SC_NPROCESSORS_ONLN);

  // Reset the tile counter each Frame
  atomic_store(&tile_counter, 0);

  // Wake up the threads
  for (int i = 0; i < total_no_of_cores_in_the_system; ++i)
    sem_post(&start_signals[i]);

  // Wait for the threads to finish
  for (int i = 0; i < total_no_of_cores_in_the_system; ++i)
    sem_wait(&done_signals[i]);

  display_render_buffer(app_state);
}

void cleanup(app_state_t *app_state) {
  threads_cleanup(thread_pool, thread_data, start_signals, done_signals);
  free(triangles_to_render);
  free(triangles_to_render_in_skybox);
  free_mesh_data(mesh);
  free_mesh_data(skybox);
  free_mesh_data(irradiance_cubemap_mesh);
  display_cleanup(app_state);
}
