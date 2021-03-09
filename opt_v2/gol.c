#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gol.h"
#include "utils.h"

#define MINDEX(i, j, mb) ((mb*i) + j)

// Static function declarations
static inline void copyOuter(char* restrict now, char* restrict future);
static inline void copyInner(char* restrict now, char* restrict future);
static inline int neighborsActive(const block_t group);
static inline void computeInner(char* restrict now, char* restrict future);
static inline void computeOuter(const block_t now, char* restrict future);
static inline void decide(const char alive, char* restrict future, const int ie,
                          const int je, const char field);


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

  // Debugging stuff
  printChars(state_b[0].data, ELEMS, ELEMS);
  printf("\n");
  printChars((*(state_b[0].top)).data, ELEMS, ELEMS);
  printf("\n");

  if (debug) {
    printf("Initial state:\n");
    printMatrix(state_b, nb, mb);
  }

  double t1 = get_wall_seconds();
  // Evolve the system
  evolve(nb, mb, nSteps);
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
 *  nb: number of rows of the matrix
 *  mb: number of columns of the matrix
 *  nSteps: number of iterations (assumed to be even)
 */
void evolve(const int nb, const int mb, const int nSteps) {
  int k, i, j;
  block_t* tmp_now;
  block_t* tmp_fut;
  for (k = 0; k < nSteps; k+=2) {

    // First iteration
    {
      for (i = 0; i < nb; i++) {
        for (j = 0; j < mb; j++) {

          tmp_now = state_b + MINDEX(i, j, mb);
          tmp_fut = other_b + MINDEX(i, j, mb);

          if (tmp_now->active) {  // State has changed (active = 1)
            tmp_fut->active = 1;
            // Recompute everything
            computeInner(tmp_now->data, tmp_fut->data);
            computeOuter(*tmp_now, tmp_fut->data);
            // No need to copy unchanged values
          } else {  // State has NOT changed (active = 0)
            if (!neighborsActive(*tmp_now)) {  // Al neighbors inactive
              // Keep everything and reset state to inactive
              tmp_fut->active = 0;
              // Hence, we must copy everything in the state matrix
              copyInner(tmp_now->data, tmp_fut->data);
              copyOuter(tmp_now->data, tmp_fut->data);
            } else {  // Some neighbors active
              tmp_fut->active = 1;
              // Recompute only the outer parts
              computeOuter(*tmp_now, tmp_fut->data);
              // Copy the inner part of the state matrix
              copyInner(tmp_now->data, tmp_fut->data);
            }
          }

        }
      }
    }

    // Second iteration
    {
      for (i = 0; i < nb; i++) {
        for (j = 0; j < mb; j++) {

          tmp_now = other_b + MINDEX(i, j, mb);
          tmp_fut = state_b + MINDEX(i, j, mb);

          if (tmp_now->active) {  // State has changed (active = 1)
            tmp_fut->active = 1;
            // Recompute everything
            computeInner(tmp_now->data, tmp_fut->data);
            computeOuter(*tmp_now, tmp_fut->data);
            // No need to copy unchanged values
          } else {  // State has NOT changed (active = 0)
            if (!neighborsActive(*tmp_now)) {  // Al neighbors inactive
              // Keep everything and reset state to inactive
              tmp_fut->active = 0;
              // Hence, we must copy everything in the state matrix
              copyInner(tmp_now->data, tmp_fut->data);
              copyOuter(tmp_now->data, tmp_fut->data);
            } else {  // Some neighbors active
              tmp_fut->active = 1;
              // Recompute only the outer parts
              computeOuter(*tmp_now, tmp_fut->data);
              // Copy the inner part of the state matrix
              copyInner(tmp_now->data, tmp_fut->data);
            }
          }

        }
      }
    }

  }
}



/*
 * Function copyOuter
 * ---------------------
 *  Copy the outer part of a block to its future
 *
 *  now: actual state matrix
 *  future: future state matrix
 */
static inline void copyOuter(char* restrict now, char* restrict future) {
  int i, j;
  // Copy first row
  for (j = 0; j < ELEMS; j++) {
    future[MINDEX(0, j, ELEMS)] = now[MINDEX(0, j, ELEMS)];
  }
  // Copy last row
  for (j = 0; j < ELEMS; j++) {
    future[MINDEX(ELEMS-1, j, ELEMS)] = now[MINDEX(ELEMS-1, j, ELEMS)];
  }
  // Copy the rest of the rows
  for (i = 1; i < ELEMS-1; i++) {
    future[MINDEX(i, 0, ELEMS)] = now[MINDEX(i, 0, ELEMS)];
    future[MINDEX(i, ELEMS-1, ELEMS)] = now[MINDEX(i, ELEMS-1, ELEMS)];
  }
}



/*
 * Function copyInner
 * ---------------------
 *  Copy the inner part of a block to its future
 *
 *  now: actual state matrix
 *  future: future state matrix
 */
static inline void copyInner(char* restrict now, char* restrict future) {
  int i, j;
  for (i = 1; i < ELEMS - 1; i++) {
    for (j = 1; j < ELEMS - 1; j++) {
      future[MINDEX(i, j, ELEMS)] = now[MINDEX(i, j, ELEMS)];
    }
  }
}



/*
 * Function neighborsActive
 * --------------------------
 *  Check if a neighbor of a block is inactive
 *
 *  group: block to check
 *
 *  returns: 1 if a neighbor is active, 0 otherwise
 */
static inline int neighborsActive(const block_t group) {
  return ((group.topleft)->active) || ((group.top)->active) ||
         ((group.topright)->active) || ((group.left)->active ||
         ((group.right)->active)) || ((group.botleft)->active) ||
         ((group.bot)->active) || ((group.botright)->active);
}



/*
 * Function computeInner
 * ---------------------
 *  Compute the future inner part of a block
 *
 *  now: actual state matrix
 *  future: future state matrix
 */
static inline void computeInner(char* restrict now, char* restrict future) {
  int i, j;
  char field;
  for (i = 1; i < ELEMS - 1; i++) {
    for (j = 1; j < ELEMS - 1; j++) {
      field = now[MINDEX(i-1, j-1, ELEMS)] + now[MINDEX(i-1, j, ELEMS)]
              + now[MINDEX(i-1, j+1, ELEMS)] + now[MINDEX(i, j-1, ELEMS)]
              + now[MINDEX(i, j, ELEMS)] + now[MINDEX(i, j+1, ELEMS)]
              + now[MINDEX(i+1, j-1, ELEMS)] + now[MINDEX(i+1, j, ELEMS)]
              + now[MINDEX(i+1, j+1, ELEMS)];
      decide(now[MINDEX(i, j, ELEMS)], future, i, j, field);
    }
  }
}



/*
 * Function computeOuter
 * ---------------------
 *  Computer the future outer part of a block
 *
 *  now: the actual block
 *  future: the future state matrix (within the block)
 */
static inline void computeOuter(const block_t now, char* restrict future) {
  int i, j;
  char field;
  // First row (i=0)
    // First column (j=0)
    field = (*(now.topleft)).data[MINDEX(ELEMS-1, ELEMS-1, ELEMS)] + (*(now.top)).data[MINDEX(ELEMS-1, 0, ELEMS)] + (*(now.top)).data[MINDEX(ELEMS-1, 1, ELEMS)]
            + (*(now.left)).data[MINDEX(0, ELEMS-1, ELEMS)] + now.data[MINDEX(0, 0, ELEMS)] + now.data[MINDEX(0, 1, ELEMS)]
            + (*(now.left)).data[MINDEX(1, ELEMS-1, ELEMS)] + now.data[MINDEX(1, 0, ELEMS)] + now.data[MINDEX(1, 1, ELEMS)];
    decide(now.data[MINDEX(0,0,ELEMS)], future, 0, 0, field);
    // Rest of columns (j=1 to ELEMS-2)
    for (j=1; j <= ELEMS - 2; j++) {
      field = (*(now.top)).data[MINDEX(ELEMS-1, j-1, ELEMS)] + (*(now.top)).data[MINDEX(ELEMS-1, j, ELEMS)] + (*(now.top)).data[MINDEX(ELEMS-1, j+1, ELEMS)]
              + now.data[MINDEX(0, j-1, ELEMS)] + now.data[MINDEX(0, j, ELEMS)] + now.data[MINDEX(0, j+1, ELEMS)]
              + now.data[MINDEX(1, j-1, ELEMS)] + now.data[MINDEX(1, j, ELEMS)] + now.data[MINDEX(1, j+1, ELEMS)];
      decide(now.data[MINDEX(0, j, ELEMS)], future, 0, j, field);
    }
    // Last column (j=ELEMS-1)
    field = (*(now.top)).data[MINDEX(ELEMS-1, ELEMS-2, ELEMS)] + (*(now.top)).data[MINDEX(ELEMS-1, ELEMS-1, ELEMS)] + (*(now.topright)).data[MINDEX(ELEMS-1, 0, ELEMS)]
            + now.data[MINDEX(0, ELEMS-2, ELEMS)] + now.data[MINDEX(0, ELEMS-1, ELEMS)] + (*(now.right)).data[MINDEX(0, 0, ELEMS)]
            + now.data[MINDEX(1, ELEMS-2, ELEMS)] + now.data[MINDEX(1, ELEMS-1, ELEMS)] + (*(now.right)).data[MINDEX(1, 0, ELEMS)];
    decide(now.data[MINDEX(0, ELEMS-1, ELEMS)], future, 0, ELEMS-1, field);
  // Other rows (i=1 to ELEMS-2)
  for (i = 1; i <= ELEMS-2; i++) {
    // First column (j=0)
    field = (*(now.left)).data[MINDEX(i-1, ELEMS-1, ELEMS)] + now.data[MINDEX(i-1, 0, ELEMS)] + now.data[MINDEX(i-1, 1, ELEMS)]
            + (*(now.left)).data[MINDEX(i, ELEMS-1, ELEMS)] + now.data[MINDEX(i, 0, ELEMS)] + now.data[MINDEX(i, 1, ELEMS)]
            + (*(now.left)).data[MINDEX(i+1, ELEMS-1, ELEMS)] + now.data[MINDEX(i+1, 0, ELEMS)] + now.data[MINDEX(i+1, 1, ELEMS)];
    decide(now.data[MINDEX(i, 0, ELEMS)], future, i, 0, field);
    // Last column (j=ELEMS-1)
    field = now.data[MINDEX(i-1, ELEMS-2, ELEMS)] + now.data[MINDEX(i-1, ELEMS-1, ELEMS)] + (*(now.right)).data[MINDEX(i-1, 0, ELEMS)]
            + now.data[MINDEX(i, ELEMS-2, ELEMS)] + now.data[MINDEX(i, ELEMS-1, ELEMS)] + (*(now.right)).data[MINDEX(i, 0, ELEMS)]
            + now.data[MINDEX(i+1, ELEMS-2, ELEMS)] + now.data[MINDEX(i+1, ELEMS-1, ELEMS)] + (*(now.right)).data[MINDEX(i+1, 0, ELEMS)];
    decide(now.data[MINDEX(i, ELEMS-1, ELEMS)], future, i, ELEMS-1, field);
  }
  // Last row (i=ELEMS-1)
    // First column (j=0)
    field = (*(now.left)).data[MINDEX(ELEMS-2, ELEMS-1, ELEMS)] + now.data[MINDEX(ELEMS-2, 0, ELEMS)] + now.data[MINDEX(ELEMS-2, 1, ELEMS)]
            + (*(now.left)).data[MINDEX(ELEMS-1, ELEMS-1, ELEMS)] + now.data[MINDEX(ELEMS-1, 0, ELEMS)] + now.data[MINDEX(ELEMS-1, 1, ELEMS)]
            + (*(now.botleft)).data[MINDEX(0, ELEMS-1, ELEMS)] + (*(now.bot)).data[MINDEX(0, 0, ELEMS)] + (*(now.bot)).data[MINDEX(0, 1, ELEMS)];
    decide(now.data[MINDEX(ELEMS-1, 0, ELEMS)], future, ELEMS-1, 0, field);
    // Rest of columns (j=1 to ELEMS-2)
    for (j=1; j <= ELEMS - 2; j++) {
      field = now.data[MINDEX(ELEMS-2, j-1, ELEMS)] + now.data[MINDEX(ELEMS-2, j, ELEMS)] + now.data[MINDEX(ELEMS-2, j+1, ELEMS)]
              + now.data[MINDEX(ELEMS-1, j-1, ELEMS)] + now.data[MINDEX(ELEMS-1, j, ELEMS)] + now.data[MINDEX(ELEMS-1, j+1, ELEMS)]
              + (*(now.bot)).data[MINDEX(0, j-1, ELEMS)] + (*(now.bot)).data[MINDEX(0, j, ELEMS)] + (*(now.bot)).data[MINDEX(0, j+1, ELEMS)];
      decide(now.data[MINDEX(ELEMS-1, j, ELEMS)], future, ELEMS-1, j, field);
    }
    // Last column (j=ELEMS-1)
    field = now.data[MINDEX(ELEMS-2, ELEMS-2, ELEMS)] + now.data[MINDEX(ELEMS-2, ELEMS-1, ELEMS)] + (*(now.right)).data[MINDEX(ELEMS-2, 0, ELEMS)]
            + now.data[MINDEX(ELEMS-1, ELEMS-2, ELEMS)] + now.data[MINDEX(ELEMS-1, ELEMS-1, ELEMS)] + (*(now.right)).data[MINDEX(ELEMS-1, 0, ELEMS)]
            + (*(now.bot)).data[MINDEX(0, ELEMS-2, ELEMS)] + (*(now.bot)).data[MINDEX(0, ELEMS-1, ELEMS)] + (*(now.botright)).data[MINDEX(0, 0, ELEMS)];
    decide(now.data[MINDEX(ELEMS-1, ELEMS-1, ELEMS)], future, ELEMS-1, ELEMS-1, field);

}



/*
 * Function decide
 * ---------------
 *  Decide wether a cell lives or dies
 *
 *  alive: current state of the cell
 *  future: pointer to the first element of the future state matrix
 *  ie: index for row
 *  je: index for column
 *  field: number of alive neighbors + the cell itself
 */
static inline void decide(const char alive, char* restrict future, const int ie,
                          const int je, const char field) {
  if (field == 3) {
    future[MINDEX(ie, je, ELEMS)] = 1;
  } else if (field == 4) {
    future[MINDEX(ie, je, ELEMS)] = alive;
  } else {
    future[MINDEX(ie, je, ELEMS)] = 0;
  }
}
