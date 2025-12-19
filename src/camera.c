#include "camera.h"

camera_t create_base_camera(void) {
  camera_t camera = {.position = {0, 0, -5.0},
                     .direction = {0, 0, 1},
                     .speed = 2.0,
                     .yaw = 0.0,
                     .pitch = 0.0,
                     .mouse_sensitivity = 0.001,
                     .delta_x_mouse_movement = 0};
  return camera;
}
