#include "appstate.h"
#include "config.h"
#include "display.h"
#include "triangle.h"
#include <stdint.h>

void setup(app_state_t *app_state);
void process_input(app_state_t *app_state);
void update(app_state_t *app_state);
void render(app_state_t *app_state);
void cleanup(app_state_t *app_state);

//////////////////////////////////////////////////////////////

triangle_t sample_triangle = {
    .vertices = {{(40.0 / 128) * WINDOW_WIDTH, (40.0 / 128) * WINDOW_HEIGHT},
                 {(80.0 / 128) * WINDOW_WIDTH, (40.0 / 128) * WINDOW_HEIGHT},
                 {(40.0 / 128) * WINDOW_WIDTH, (80.0 / 128) * WINDOW_HEIGHT}},
    .colors = {{0xFF, 0X00, 0x00}, {0x00, 0XFF, 0x00}, {0x00, 0X00, 0xFF}}};

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

void setup(app_state_t *app_state) { display_init(app_state); }

void process_input(app_state_t *app_state) {}

void update(app_state_t *app_state) {}

void render(app_state_t *app_state) {
  display_clear_buffer(app_state, 0xFF000000);

  ////////////////////////////////////////////////////////////
  fill_triangle(sample_triangle, app_state);
  /////////////////////////////////////////
  //////////////////////

  display_render_buffer(app_state);
}

void cleanup(app_state_t *app_state) { display_cleanup(app_state); }
