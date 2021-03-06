#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "utils.h"



/*
 * Function allocateMatrix
 * -----------------------
 *  Allocate memory for a matrix
 *
 *  n: number of rows of the matrix
 *  m: number of columns of the matrix
 *
 *  returns: a pointer to the first element of the matrix
 */
int** allocateMatrix(int n, int m) {
  int** mat = (int**) malloc(n * sizeof(int*));
  int i;
  for (i = 0; i < n; i++) {
    mat[i] = (int*) malloc(m * sizeof(int));
  }
  return mat;
}



/*
 * Function freeMatrix
 * -----------------------
 *  Free memory occupied by a matrix
 *
 *  mat: pointer to the first element of the matrix
 *  n: number of rows of the matrix
 *  m: number of columns of the matrix
 */
void freeMatrix(int** mat, int n, int m) {
  int i;
  for (i = 0; i < n; i++) {
    free(mat[i]);
  }
  free(mat);
}



/*
 * Function createInitialState
 * ---------------------------
 *  Create an initial state for the Game of Life
 *
 *  mat: pointer to the first element of the state matrix
 *  n: number of rows of the matrix
 *  m: number of columns of the matrix
 *  prob: probability of a cell being alive
 */
void createInitialState(int** mat, int n, int m, double prob) {
  int i, j;
  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) {
      mat[i][j] = cRandom() <= prob ? 1 : 0;
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
double cRandom() {
  // https://stackoverflow.com/questions/6218399/how-to-generate-a-random-number-between-0-and-1
  return (double) rand() / (double) RAND_MAX;
}



/*
 * Function printMatrix
 * --------------------
 *  Print matrix to console
 *
 *  mat: pointer to the first element of the matrix
 *  n: number of rows of the matrix
 *  m: number of columns of the matrix
 */
void printMatrix(int** mat, int n, int m) {
  int i, j;
  for (i = 0; i < n; i++) {
    printf("[ ");
    for (j = 0; j < m; j++) {
      printf("%d ", mat[i][j]);
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
