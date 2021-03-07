#include <stdio.h>
#include <stdlib.h>
#include "gol.h"
#include "utils.h"

char** state;
char** other;

int main(int argc, char const *argv[]) {

  // Check that arguments are provided
  if (argc != 5) {
    printf("Usage: %s n m prob nSteps\n", argv[0]);
    return -1;
  }

  // Parse arguments
  const int n = atoi(argv[1]);
  const int m = atoi(argv[2]);
  const double prob = atof(argv[3]);
  const int nSteps = atoi(argv[4]);
  printf("%d %d %lf %d\n", n, m, prob, nSteps);

  // Check that arguments are valid
  if (n <= 0 || m <= 0 || nSteps <= 0 || prob < 0 || prob > 1) {
    printf("Usage:\n  n, m and nSteps must be positive integers\n  prob must be in range [0, 1]");
    return -1;
  }

  // Initialize data structures
  state = allocateMatrix(n, m);
  other = allocateMatrix(n, m);

  // Create initial state
  createInitialState(state, n, m, prob);

  // printMatrix(state, n, m);

  double t1 = get_wall_seconds();
  // Evolve the system
  evolve(state, n, m, nSteps);
  printf("%lf\n", get_wall_seconds() - t1);

  // Free data structures
  freeMatrix(state, n, m);
  freeMatrix(other, n, m);

  return 0;
}


/*
 * Function evolve
 * ---------------
 *  Evolve the game state for a given number of iterations
 *
 *  state: pointer to the first element of the state matrix
 *  nRows: number of rows of the matrix
 *  nCols: number of columns of the matrix
 *  nSteps: number of iterations (assumed to be even)
 */
void evolve(char** state, const int nRows, const int nCols, const int nSteps) {
  int k, i, j;
  for (k = 0; k < nSteps; k+=2) {

    // First row

    // Other rows

    // Last row

  }
}
