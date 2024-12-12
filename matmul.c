///////////////////////////////////////
/// 
/// compile with
/// gcc matrix_multiply_comp.c -o com_mm -lm -O3 -lrt
/// 
///////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define MATRIX_SIZE 32
#define BILLION 1000000000ULL


int main() {
  // Initialize variables
  struct timespec start, mid1, mid2, end;
  int i, j, k;
//   clock_gettime(CLOCK_MONOTONIC, &start);
  int** m1 = (int**)malloc(MATRIX_SIZE * sizeof(int*));
  int** m2 = (int**)malloc(MATRIX_SIZE * sizeof(int*));
  int** res = (int**)malloc(MATRIX_SIZE * sizeof(int*));

  for (i = 0; i < MATRIX_SIZE; i++) {
    m1[i] = (int*)malloc(MATRIX_SIZE * sizeof(int));
    m2[i] = (int*)malloc(MATRIX_SIZE * sizeof(int));
    res[i] = (int*)malloc(MATRIX_SIZE * sizeof(int));
  }

  // Initialize matrix values
  for (i = 0; i < MATRIX_SIZE; i++) {
    for (j = 0; j < MATRIX_SIZE; j++) {
      m1[i][j] = j;
      m2[i][j] = j;
      res[i][j] = 0;
    }
  }
//   clock_gettime(CLOCK_MONOTONIC, &mid1);

  // Run the N^3 matrix multiplication algorithm
  int accum = 0;

  for (i = 0; i < MATRIX_SIZE; i++) {
    for (j = 0; j < MATRIX_SIZE; j++) {
      accum = 0;
      for (k = 0; k < MATRIX_SIZE; k++) {
        accum += (m1[k][j] * m2[i][k]);
      }
      res[i][j] = accum;
    }
  }
//   clock_gettime(CLOCK_MONOTONIC, &mid2);

  // Print matrix result
  for (i = 0; i < MATRIX_SIZE; i++) {
    for (j = 0; j < MATRIX_SIZE; j++) {
      printf(" %d ", res[i][j]);
    }
    printf("\n");
  }



  // Ensure no leaks
  for (i = 0; i < MATRIX_SIZE; i++) {
    free(m1[i]);
    free(m2[i]);
    free(res[i]);
  }
  free(m1);
  free(m2);
  free(res);
//   clock_gettime(CLOCK_MONOTONIC, &end);

  // Get timing information
  unsigned long long setup_time = ((BILLION * mid1.tv_sec) + mid1.tv_nsec) - ((BILLION * start.tv_sec) + start.tv_nsec);
  unsigned long long calc_time = ((BILLION * mid2.tv_sec) + mid2.tv_nsec) - ((BILLION * mid1.tv_sec) + mid1.tv_nsec);
  unsigned long long clean_time = ((BILLION * end.tv_sec) + end.tv_nsec) - ((BILLION * mid2.tv_sec) + mid2.tv_nsec);
  double setup_time_s = (double)setup_time / BILLION;
  double calc_time_s = (double)calc_time / BILLION;
  double clean_time_s = (double)clean_time / BILLION;
  printf("\nSetup time (s): %lf\nCalc time (s): %lf\nClean time (s): %lf\nTotal time elapsed (s): %lf\n",
    setup_time_s, calc_time_s, clean_time_s, setup_time_s + calc_time_s + clean_time_s);

  return 0;
}