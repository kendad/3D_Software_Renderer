#include "camera.h"

camera_t create_base_camera(void) {
  camera_t camera = {.position = {0, 0, -5}, .direction = {0, 0, 1}};
  return camera;
}
