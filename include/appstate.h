#pragma once
#include "lights.h"
#include "vector.h"
#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;

  uint32_t *color_buffer;
  float *z_buffer;
  SDL_Texture *color_buffer_texture;

  bool is_running;
  float previous_frame_time;
  float delta_time;
} app_state_t;

typedef struct {
  light_t *lights;
  int *total_lights_in_scene;
  vec3_t *camera_position;
} scene_info_t;
