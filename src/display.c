#include "display.h"
#include "config.h"
#include <SDL.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

void display_init(app_state_t *app_state) {
  app_state->is_running = true;

  // Initalize SDL
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDLd\n");
    app_state->is_running = false;
  }

  // Create a window
  app_state->window =
      SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  if (!app_state->window) {
    fprintf(stderr, "Error creating SDL Window\n");
    app_state->is_running = false;
  }

  // create a renderer
  app_state->renderer = SDL_CreateRenderer(app_state->window, -1, 0);
  if (!app_state->renderer) {
    fprintf(stderr, "Error creating SDL Renderer\n");
    app_state->is_running = false;
  }

  // create a texture
  app_state->color_buffer_texture = SDL_CreateTexture(
      app_state->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
      WINDOW_WIDTH, WINDOW_HEIGHT);
  if (!app_state->color_buffer_texture) {
    fprintf(stderr, "Error creating SDL Texture\n");
    app_state->is_running = false;
  }

  // Allocate memory space for the color buffer
  app_state->color_buffer =
      (uint32_t *)calloc(WINDOW_WIDTH * WINDOW_HEIGHT, sizeof(uint32_t));
}

void display_clear_buffer(app_state_t *app_state, uint32_t color) {
  for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; ++i) {
    app_state->color_buffer[i] = color;
  }
}

void display_render_buffer(app_state_t *app_state) {
  // copy the color data from the color buffer to the texture
  SDL_UpdateTexture(app_state->color_buffer_texture, NULL,
                    app_state->color_buffer,
                    (int)(WINDOW_WIDTH * sizeof(uint32_t)));

  // Put the texture data onto the renderer
  SDL_RenderCopy(app_state->renderer, app_state->color_buffer_texture, NULL,
                 NULL);

  // Render the updated texture onto the screen
  SDL_RenderPresent(app_state->renderer);
}

void display_cleanup(app_state_t *app_state) {
  // Free Up allocated memory space
  free(app_state->color_buffer);
  SDL_DestroyTexture(app_state->color_buffer_texture);
  SDL_DestroyRenderer(app_state->renderer);
  SDL_DestroyWindow(app_state->window);
  SDL_Quit();
}

void display_draw_pixel(int x, int y, uint32_t color, app_state_t *app_state) {
  app_state->color_buffer[x + (WINDOW_WIDTH * y)] = color;
}
