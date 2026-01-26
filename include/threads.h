#pragma once

#include "appstate.h"
#include "lights.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdbool.h>

typedef struct {
  app_state_t *app_state;
  atomic_int *tile_counter;
  sem_t *start_signal;
  sem_t *done_signal;
  bool *is_main_thread_running;
  // all the properties required to render a triangle
  triangle_t *triangles_to_render;
  triangle_t *triangles_to_render_in_skybox;
  int *triangles_to_render_count;
  int *triangles_to_render_in_skybox_count;
  texture_t *texture_data;
  texture_t *texture_data_for_skybox;
  texture_t *radiance_texture_data;
  texture_t *irradiance_texture_data;
  texture_t *LUT_texture_data;
  light_t *lights;
  int *total_lights_in_scene;
  vec3_t *camera_position;
} thread_t;

void threads_initialize(
    app_state_t *app_state, pthread_t **thread_pool, thread_t **thread_data,
    sem_t **start_signals, sem_t **done_signals, atomic_int *tile_counter,
    bool *is_main_thread_running, triangle_t *triangles_to_render,
    triangle_t *triangles_to_render_in_skybox, int *triangles_to_render_count,
    int *triangles_to_render_in_skybox_count, texture_t *texture_data,
    texture_t *texture_data_for_skybox, texture_t *radiance_texture_data,
    texture_t *irradiance_texture_data, texture_t *LUT_texture_data,
    light_t *lights, int *total_lights_in_scene, vec3_t *camera_position);

void threads_cleanup(pthread_t *thread_pool, thread_t *thread_data,
                     sem_t *start_signals, sem_t *done_signals);

void *thread_render(void *arg);
