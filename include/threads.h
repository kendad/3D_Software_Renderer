#pragma once

#include "appstate.h"
#include "triangle.h"
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
  material_t *base_material;
  material_t *skybox_material;
  scene_info_t *scene_info;
} thread_t;

void threads_initialize(app_state_t *app_state, pthread_t **thread_pool,
                        thread_t **thread_data, sem_t **start_signals,
                        sem_t **done_signals, atomic_int *tile_counter,
                        bool *is_main_thread_running, material_t *base_material,
                        material_t *skybox_material, scene_info_t *scene_info);

void threads_cleanup(pthread_t *thread_pool, thread_t *thread_data,
                     sem_t *start_signals, sem_t *done_signals);

void *thread_render(void *arg);
