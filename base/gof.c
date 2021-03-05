#include <stdio.h>
#include <stdlib.h>
#include "gof.h"

int main(int argc, char const *argv[]) {

  // Check that arguments are provided
  if (argc != 4) {
    printf("Usage: %s n m nSteps\n", argv[0]);
    return -1;
  }

  // Parse arguments
  const int n = atoi(argv[1]);
  const int m = atoi(argv[2]);
  const int nSteps = atoi(argv[3]);
  printf("%d %d %d\n", n, m, nSteps);

  // Check that arguments are valid
  if (n <= 0 || m <= 0 || nSteps <= 0) {
    printf("Usage: n, m and nSteps must be positive integers\n");
    return -1;
  }

  // Initialize data structures

  // Free data structures

  return 0;
}
