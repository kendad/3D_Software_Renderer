#include "matrix.h"
#include "vector.h"
#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

uint32_t *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;

int window_width = 800;
int window_height = 600;

bool init_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initialzing SDL\n");
    return false;
  }

  // create a window
  window =
      SDL_CreateWindow("3D Rasterizer", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);

  if (!window) {
    fprintf(stderr, "Error creatign SDL Window\n");
    return false;
  }

  // create a renderer
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    fprintf(stderr, "Error creating SDL Renderer\n");
    return false;
  }

  // create a texture
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);

  if (!color_buffer_texture) {
    fprintf(stderr, "Error Creating SDL Texture\n");
    return false;
  }

  color_buffer =
      (uint32_t *)calloc(window_width * window_height, sizeof(uint32_t));

  return true;
}

void render(void) {

  // clear the color buffer with red
  for (int i = 0; i < window_width * window_height; ++i) {
    color_buffer[i] = 0xFF0000FF;
  }

  // copy the colors from the color buffer to the texture
  SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
                    (int)(window_width * sizeof(uint32_t)));

  // Puts the texture onto the renderer
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);

  // Renders the updated renderer onto the screen
  SDL_RenderPresent(renderer);
}

int main() {
  bool isRunning = init_window();
  while (isRunning) {
    render();
  }
  // cleanup
  free(color_buffer);
  SDL_DestroyTexture(color_buffer_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
