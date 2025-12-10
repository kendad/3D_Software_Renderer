#include "vector.h"
#include <stdio.h>

int main() {
  vec3_t someVector = vec3_new(0.1, 0.2, 1.9);
  vec3_t otherVector = vec3_new(0.1, 0.2, 0.4);
  vec3_t new_cross = vec3_cross(someVector, otherVector);
  printf("%0.3f %0.3f %0.3f \n", new_cross.x, new_cross.y, new_cross.z);
  return 0;
}
