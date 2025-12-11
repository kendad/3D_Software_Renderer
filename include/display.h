#pragma once

#include "appstate.h"
#include <stdint.h>

void display_init(app_state_t *app_state);
void display_clear_buffer(app_state_t *app_state, uint32_t color);
void display_render_buffer(app_state_t *app_state);
void display_cleanup(app_state_t *app_state);

void display_draw_pixel(int x, int y, uint32_t color, app_state_t *app_state);
