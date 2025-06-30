#include "snitch_hw_vector_add.h"
#include <stdio.h>

void snitch_hw_vector_add(const float* a, const float* b, float* out, int N) {
  printf("[snitch_hw_vector_add] called with N = %d\n", N);
  for (int i = 0; i < N; ++i) {
    out[i] = a[i] + b[i];
  }
}