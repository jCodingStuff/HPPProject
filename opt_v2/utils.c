#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "gol.h"
#include "utils.h"

#define MINDEX(i, j, mb) ((mb*i) + j)

// Forward declaration of static methods
static inline double cRandom();



/*
 * Function allocateMatrix
 * -----------------------
 *  Allocate memory for a matrix of char
 *
 *  nRows: number of rows of the matrix
 *  nCols: number of columns of the matrix
 *
 *  returns: a pointer to the first element of the matrix
 */
char* allocateMatrix(const int nRows, const int nCols) {
  return (char*) malloc(nRows * nCols * sizeof(char));
}



/*
 * Function initBlocks
 * -------------------
 *  Allocate memory for matrix of block_t and initialize its contents
 *
 *  mat: state matrix of char
 *  nb: number of blocks per row
 *  mb: number of blocks per column
 *
 *  returns: a pointer to the first element of the block matrix
 */
block_t* initBlocks(const int nb, const int mb) {
  // Allocate memory for the blocks
  block_t* mat_b = (block_t*) malloc(nb * mb * sizeof(block_t));
  // Set up the blocks
  int i, j;
  for (i = 0; i < nb; i++) {
    for (j = 0; j < mb; j++) {
      mat_b[MINDEX(i, j, mb)].active = 1;  // Set as active for initial seed
      // Set non-diagonal neighbors
      mat_b[MINDEX(i, j, mb)].top = i == 0 ? (mat_b + MINDEX(nb-1, j, mb)) : (mat_b + MINDEX(i-1, j, mb));
      mat_b[MINDEX(i, j, mb)].bot = i == nb-1 ? (mat_b + MINDEX(0, j, mb)) : (mat_b + MINDEX(i+1, j, mb));
      mat_b[MINDEX(i, j, mb)].left = j == 0 ? (mat_b + MINDEX(i, mb-1, mb)) : (mat_b + MINDEX(i, j-1, mb));
      mat_b[MINDEX(i, j, mb)].right = j == mb-1 ? (mat_b + MINDEX(i, 0, mb)) : (mat_b + MINDEX(i, j+1, mb));
      // Set diagonal neighbors
      // topleft
      {
        if (i == 0 && j == 0) {
          mat_b[MINDEX(i, j, mb)].topleft = mat_b + MINDEX(nb-1, mb-1, mb);
        } else if (i == 0) {
          mat_b[MINDEX(i, j, mb)].topleft = mat_b + MINDEX(nb-1, j-1, mb);
        } else if (j == 0) {
          mat_b[MINDEX(i, j, mb)].topleft = mat_b + MINDEX(i-1, mb-1, mb);
        } else {
          mat_b[MINDEX(i, j, mb)].topleft = mat_b + MINDEX(i-1, j-1, mb);
        }
      }
      // topright
      {
        if (i == 0 && j == mb-1) {
          mat_b[MINDEX(i, j, mb)].topright = mat_b + MINDEX(nb-1, 0, mb);
        } else if (i == 0) {
          mat_b[MINDEX(i, j, mb)].topright = mat_b + MINDEX(nb-1, j+1, mb);
        } else if (j == mb-1) {
          mat_b[MINDEX(i, j, mb)].topright = mat_b + MINDEX(i-1, 0, mb);
        } else {
          mat_b[MINDEX(i, j, mb)].topright = mat_b + MINDEX(i-1, j+1, mb);
        }
      }
      // botleft
      {
        if (i == nb-1 && j == 0) {
          mat_b[MINDEX(i, j, mb)].botleft = mat_b + MINDEX(0, mb-1, mb);
        } else if (i == nb-1) {
          mat_b[MINDEX(i, j, mb)].botleft = mat_b + MINDEX(0, j-1, mb);
        } else if (j == 0) {
          mat_b[MINDEX(i, j, mb)].botleft = mat_b + MINDEX(i+1, mb-1, mb);
        } else {
          mat_b[MINDEX(i, j, mb)].botleft = mat_b + MINDEX(i+1, j-1, mb);
        }
      }
      // botright
      {
        if (i == nb-1 && j == mb-1) {
          mat_b[MINDEX(i, j, mb)].botright = mat_b + MINDEX(0, 0, mb);
        } else if (i == nb-1) {
          mat_b[MINDEX(i, j, mb)].botright = mat_b + MINDEX(0, j+1, mb);
        } else if (j == mb-1) {
          mat_b[MINDEX(i, j, mb)].botright = mat_b + MINDEX(i+1, 0, mb);
        } else {
          mat_b[MINDEX(i, j, mb)].botright = mat_b + MINDEX(i+1, j+1, mb);
        }
      }
    }
  }
  return mat_b;
}



/*
 * Function createInitialState
 * ---------------------------
 *  Create an initial state for the Game of Life
 *
 *  mat: pointer to the first element of the state matrix of block_t
 *  nb: number of rows of the matrix
 *  mb: number of columns of the matrix
 *  prob: probability of a cell being alive
 */
void createInitialState(block_t* restrict mat, const int nb, const int mb,
                        const double prob) {
  int i, j, ie, je;
  for (i = 0; i < nb; i++) {
    for (ie = 0; ie < ELEMS; ie++) {
      for (j = 0; j < mb; j++) {
        for (je = 0; je < ELEMS; je++) {
          mat[MINDEX(i, j, mb)].data[MINDEX(ie, je, ELEMS)] = cRandom() <= prob ? 1 : 0;
        }
      }
    }
  }
}



/*
 * Function cRandom
 * ----------------
 *  Generate a uniform random number in range [0, 1]
 *
 *  returns: the generated number
 */
static inline double cRandom() {
  // https://stackoverflow.com/questions/6218399/how-to-generate-a-random-number-between-0-and-1
  return (double) rand() / (double) RAND_MAX;
}



/*
 * Function printMatrix
 * --------------------
 *  Print matrix of block_t to console
 *
 *  mat: pointer to the first element of the matrix
 *  nb: number of rows of the matrix
 *  mb: number of columns of the matrix
 */
void printMatrix(block_t* restrict mat, const int nb, const int mb) {
  int i, j, ie, je;
  for (i = 0; i < nb; i++) {
    for (ie = 0; ie < ELEMS; ie++) {
      printf("[ ");
      for (j = 0; j < mb; j++) {
        for (je = 0; je < ELEMS; je++) {
          printf("%d ", mat[MINDEX(i, j, mb)].data[MINDEX(ie, je, ELEMS)]);
        }
      }
      printf("]\n");
    }
  }
}



/*
 * Function printChars
 * --------------------
 *  Print matrix of chars to console
 *
 *  mat: pointer to the first element of the matrix
 *  n: number of rows of the matrix
 *  m: number of columns of the matrix
 */
void printChars(char* restrict mat, const int n, const int m) {
  int i, j;
  for (i = 0; i < n; i++) {
    printf("[ ");
    for (j = 0; j < m; j++) {
      printf("%d ", mat[MINDEX(i, j, m)]);
    }
    printf("]\n");
  }
}



/*
* Function: get_wall_seconds
* ----------------------
*  Fetch the current wall time
*
*  returns: the current wall time
*/
double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}
