#include "matrix.h"
#include "vector.h"
#include <stdio.h>

int main() {
  mat4_t id1 = mat4_make_identity();
  mat4_t id2 = mat4_make_identity();
  mat4_t result = mat4_mul_mat4(id1, id2);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; j++) {
      printf("%0.3f ", result.data[i][j]);
    }
    puts("\n");
  }
  return 0;
}
