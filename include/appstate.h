#pragma once
#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;

  uint32_t *color_buffer;
  SDL_Texture *color_buffer_texture;

  bool is_running;
  float previous_frame_time;
  float delta_time;
} app_state_t;
