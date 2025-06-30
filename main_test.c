// [5] Test harness (main_test.c)
#include <stdio.h>
#include "snitch_hw_vector_add.h"

int main() {
  float a[4] = {1.0, 2.0, 3.0, 4.0};
  float b[4] = {10.0, 20.0, 30.0, 40.0};
  float c[4] = {0};
  snitch_hw_vector_add(a, b, c, 4);
  for (int i = 0; i < 4; ++i) {
    printf("c[%d] = %f\n", i, c[i]);
  }
  return 0;
}
