#include "threads.h"
#include "appstate.h"
#include "config.h"
#include "triangle.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void threads_initialize(app_state_t *app_state, pthread_t **thread_pool,
                        thread_t **thread_data, sem_t **start_signals,
                        sem_t **done_signals, atomic_int *tile_counter,
                        bool *is_main_thread_running, material_t *base_material,
                        material_t *skybox_material, scene_info_t *scene_info) {

  // Get the total no of cores in the system
  int total_no_of_cores_in_the_system = sysconf(_SC_NPROCESSORS_ONLN);
  *thread_pool = malloc(sizeof(pthread_t) * total_no_of_cores_in_the_system);
  *thread_data = malloc(sizeof(thread_t) * total_no_of_cores_in_the_system);
  *start_signals = malloc(sizeof(sem_t) * total_no_of_cores_in_the_system);
  *done_signals = malloc(sizeof(sem_t) * total_no_of_cores_in_the_system);

  // start the threads
  for (int i = 0; i < total_no_of_cores_in_the_system; ++i) {
    sem_init(&(*start_signals)[i], 0, 0);
    sem_init(&(*done_signals)[i], 0, 0);
    thread_t thread_data_for_current_index = {
        .app_state = app_state,
        .tile_counter = tile_counter,
        .start_signal = &(*start_signals)[i],
        .done_signal = &(*done_signals)[i],
        .is_main_thread_running = is_main_thread_running,
        .base_material = base_material,
        .skybox_material = skybox_material,
        .scene_info = scene_info};

    (*thread_data)[i] = thread_data_for_current_index;
    pthread_create(&(*thread_pool)[i], NULL, thread_render, &(*thread_data)[i]);
  }
}

void threads_cleanup(pthread_t *thread_pool, thread_t *thread_data,
                     sem_t *start_signals, sem_t *done_signals) {
  // cleanup all the threads and free the dynamically allocated memory
  int total_no_of_cores_in_the_system = sysconf(_SC_NPROCESSORS_ONLN);
  for (int i = 0; i < total_no_of_cores_in_the_system; ++i) {
    *thread_data[i].is_main_thread_running = false;
    sem_post(&start_signals[i]);
    pthread_join(thread_pool[i], NULL);
    sem_destroy(&start_signals[i]);
    sem_destroy(&done_signals[i]);
  }
  free(thread_pool);
  free(thread_data);
  free(start_signals);
  free(done_signals);
}

void *thread_render(void *arg) {
  thread_t *thread_data = (thread_t *)arg;

  // calculate the total tiles in the X&Y directions
  int total_tiles_in_x = (WINDOW_WIDTH + TILE_SIZE - 1) / TILE_SIZE;
  int total_tiles_in_y = (WINDOW_HEIGHT + TILE_SIZE - 1) / TILE_SIZE;
  int total_tiles = total_tiles_in_x * total_tiles_in_y;

  while (true) {
    // wait till the start signal is given
    sem_wait(thread_data->start_signal);
    // end the thread if the main thread stops running
    if (*thread_data->is_main_thread_running == false)
      break;

    // The main working that is kept alive untill all the tiles has been
    // rendered
    while (true) {
      // get which tile to render from the global tile pool
      // once you get the tile id just increment this counter so that the next
      // thread can up for the tiles that has still not yet been rendered
      int tile_id = atomic_fetch_add(thread_data->tile_counter, 1);
      // if tile counter exceeds the total tiles then end the thread
      if (tile_id >= total_tiles)
        break;

      // do all the calculations here
      // calculate the min(top_left) and max(bottom_right) of the tile boundary
      int tile_x_min = (tile_id % total_tiles_in_x) * TILE_SIZE;
      int tile_y_min = (tile_id / total_tiles_in_x) * TILE_SIZE;
      int tile_x_max = tile_x_min + TILE_SIZE - 1;
      int tile_y_max = tile_y_min + TILE_SIZE - 1;

      bounding_box_t tile_bounding_box = {.x_min = tile_x_min,
                                          .y_min = tile_y_min,
                                          .x_max = tile_x_max,
                                          .y_max = tile_y_max};

      // render Mesh
      for (int i = 0;
           i < *thread_data->base_material->triangles_to_render_count; ++i) {
        draw_triangle_fill_tiled_with_lighting_effect(
            thread_data->base_material->triangles_to_render[i],
            thread_data->base_material, thread_data->scene_info,
            tile_bounding_box, thread_data->app_state);
      }

      // render Skybox
      for (int i = 0;
           i < *thread_data->skybox_material->triangles_to_render_count; ++i) {
        draw_triangle_fill_tiled_with_lighting_effect(
            thread_data->skybox_material->triangles_to_render[i],
            thread_data->skybox_material, thread_data->scene_info,
            tile_bounding_box, thread_data->app_state);
      }
    }

    // send the signal that the work has been completed
    sem_post(thread_data->done_signal);
  }
  return NULL;
}
