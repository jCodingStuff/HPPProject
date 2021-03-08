#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gol.h"
#include "utils.h"



int** state;
int** other;
int** tmp;  // Temporal pointer


int main(int argc, char const *argv[]) {

  // Check that arguments are provided
  if (argc != 5) {
    printf("Usage: %s n m prob nSteps\n", argv[0]);
    return -1;
  }

  // Parse arguments
  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  double prob = atof(argv[3]);
  int nSteps = atoi(argv[4]);
  // printf("%d %d %lf %d\n", n, m, prob, nSteps);

  // Check that arguments are valid
  if (n <= 0 || m <= 0 || nSteps <= 0 || prob < 0 || prob > 1) {
    printf("Usage:\n  n, m and nSteps must be positive integers\n  prob must be in range [0, 1]\n");
    return -1;
  }

  // Initialize arbitrary seed for random numbers
  // srand(time(NULL));
  srand(1);

  // Initialize data structures
  // state = allocateMatrix(n, m);
  // other = allocateMatrix(n, m);

  // Create initial state
  createInitialState(state, n, m, prob);

  // printf("Initial state:\n");
  // printMatrix(state, n, m);

  double t1 = get_wall_seconds();
  // Evolve the system
  evolve(n, m, nSteps);
  // Print time it took to do the simulation
  printf("%lf\n", get_wall_seconds() - t1);

  // printf("Final state:\n");
  // printMatrix(state, n, m);

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
 *  n: number of rows of the matrix
 *  m: number of columns of the matrix
 *  nSteps: number of iterations (assumed to be even)
 */
void evolve(int n, int m, int nSteps) {
  int k, i, j;
  int neighbors;
  int topleft, top, topright, left, right, botleft, bot, botright;
  for (k = 0; k < nSteps; k++) {

    // Generate next state
    for (i = 0; i < n; i++) {
      for (j = 0; j < m; j++) {
        // Select the correct elements for non-diagonal
        top = i == 0 ? state[n-1][j] : state[i-1][j];
        bot = i == n-1 ? state[0][j] : state[i+1][j];
        left = j == 0 ? state[i][m-1] : state[i][j-1];
        right = j == m-1 ? state[i][0] : state[i][j+1];
        // topleft
        if (i == 0 && j == 0) {
          topleft = state[n-1][m-1];
        } else if (i == 0) {
          topleft = state[n-1][j-1];
        } else if (j == 0) {
          topleft = state[i-1][m-1];
        } else {
          topleft = state[i-1][j-1];
        }
        // topright
        if (i == 0 && j == m-1) {
          topright = state[n-1][0];
        } else if (i == 0) {
          topright = state[n-1][j+1];
        } else if (j == m-1) {
          topright = state[i-1][0];
        } else {
          topright = state[i-1][j+1];
        }
        // botleft
        if (i == n-1 && j == 0) {
          botleft = state[0][m-1];
        } else if (i == n-1) {
          botleft = state[0][j-1];
        } else if (j == 0) {
          botleft = state[i+1][m-1];
        } else {
          botleft = state[i+1][j-1];
        }
        // botright
        if (i == n-1 && j == m-1) {
          botright = state[0][0];
        } else if (i == n-1) {
          botright = state[0][j+1];
        } else if (j == m-1) {
          botright = state[i+1][0];
        } else {
          botright = state[i+1][j+1];
        }
        // Count neighbors and decide
        neighbors = topleft + top + topright + left + right + botleft + bot + botright;
        decide(state[i][j], other, i, j, neighbors);
      }
    }

    // Make state point to other and other point to state
    tmp = state;
    state = other;
    other = tmp;

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
 *  neighbors: number of alive neighbors
 */
void decide(int alive, int** future, int i, int j, int neighbors) {
  if (alive && neighbors < 2) {
    future[i][j] = 0;
  } else if (alive && (neighbors == 2 || neighbors == 3)) {
    future[i][j] = 1;
  } else if (alive && neighbors > 3) {
    future[i][j] = 0;
  } else if (!alive && neighbors == 3) {
    future[i][j] = 1;
  } else {
    future[i][j] = 0;
  }
}
