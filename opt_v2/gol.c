#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gol.h"
#include "utils.h"

#define MINDEX(i, j, mb) ((mb*i) + j)

// Static function declarations
static inline void decide(const char alive, char** restrict future, const int i,
                          const int j, const char field);


block_t* restrict state_b; // State blocks at even times (0, 2, 4, etc.)
block_t* restrict other_b; // State blocks at odd times (1, 3, 5, etc.)



int main(int argc, char const *argv[]) {

  // Check that arguments are provided
  if (argc != 7) {
    printf("Usage: %s n m prob nSteps seed debug\n", argv[0]);
    return -1;
  }

  // Parse arguments
  const int n = atoi(argv[1]);
  const int m = atoi(argv[2]);
  const double prob = atof(argv[3]);
  const int nSteps = atoi(argv[4]);
  const int seed = atoi(argv[5]);
  const int debug = atoi(argv[6]);
  // printf("%d %d %lf %d\n", n, m, prob, nSteps);

  // Check that arguments are valid
  if (n <= 0 || m <= 0 || nSteps <= 0 || prob < 0 || prob > 1) {
    printf("Usage:\n  n, m and nSteps must be positive integers\n  prob must be in range [0, 1]\n");
    return -1;
  }

  // Initialize arbitrary seed for random numbers (or not!)
  if (seed < 0) {
    srand(time(NULL));
  } else {
    srand((unsigned int) seed);
  }

  // Compute bounds for the block matrices (assuming perfect integer division)
  const int nb = n / ELEMS;
  const int mb = m / ELEMS;

  // Initialize the blocks
  state_b = initBlocks(nb, mb);
  other_b = initBlocks(nb, mb);

  // Create initial state
  createInitialState(state_b, nb, mb, prob);

  if (debug) {
    printf("Initial state:\n");
    printMatrix(state_b, nb, mb);
  }

  double t1 = get_wall_seconds();
  // Evolve the system
  evolve(n, m, nSteps);
  // Print time it took to do the simulation
  printf("%lf\n", get_wall_seconds() - t1);

  if (debug) {
    printf("Final state:\n");
    printMatrix(state_b, nb, mb);
  }

  // Free data structures
  free(state_b);
  free(other_b);

  return 0;
}



/*
 * Function evolve
 * ---------------
 *  Evolve the game state for a given number of iterations
 *
 *  n: number of rows of the matrix
 *  m: number of columns of the matrix
 *  nSteps: number of iterations (assumed to be even)
 */
void evolve(const int n, const int m, const int nSteps) {
  int k, i, j;
  char field;
  for (k = 0; k < nSteps; k+=2) {

  }
}


/*
 * Function decide
 * ---------------
 *  Decide wether a cell lives or dies
 *
 *  alive: current state of the cell
 *  future: pointer to the first element of the future state matrix
 *  i: index for row
 *  j: index for column
 *  field: number of alive neighbors + the cell itself
 */
static inline void decide(const char alive, char** restrict future, const int i,
                          const int j, const char field) {
  if (field == 3) {
    future[i][j] = 1;
  } else if (field == 4) {
    future[i][j] = alive;
  } else {
    future[i][j] = 0;
  }
}
