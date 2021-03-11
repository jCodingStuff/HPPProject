#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <omp.h>
#include "utils.h"
#include "gol.h"


// Forward declaration of static methods
static inline double cRandom();



/*
 * Function allocateMatrix
 * -----------------------
 *  Allocate memory for a matrix
 *
 *  nRows: number of rows of the matrix
 *  nCols: number of columns of the matrix
 *
 *  returns: a pointer to the first element of the matrix
 */
char** allocateMatrix(const int nRows, const int nCols) {
  char** mat = (char**) malloc(nRows * sizeof(char*));
  int i;
  for (i = 0; i < nRows; i++) {
    mat[i] = (char*) malloc(nCols * sizeof(char));
  }
  return mat;
}



/*
 * Function freeMatrix
 * -----------------------
 *  Free memory occupied by a matrix
 *
 *  mat: pointer to the first element of the matrix
 *  nRows: number of rows of the matrix
 *  nCols: number of columns of the matrix
 */
void freeMatrix(char** restrict mat, const int nRows, const int nCols) {
  int i;
  for (i = 0; i < nRows; i++) {
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
 *  nThreads: number of threads
 *  threadData: pointer to the first element of the array containing thread data
 */
void createInitialState(char** restrict mat, const int n, const int m,
                        const double prob, const int nThreads,
                        tdata_t* restrict threadData) {
  int i, j;
  int tid = omp_get_thread_num();

  if (tid == 0) {
    for (j = 0; j < m; j++) {
      mat[0][j] = cRandom() <= prob ? 1 : 0;
    }
  }
  if (tid == nThreads - 1) {  // Cannot use else if in case there is just 1 thread!
    for (j = 0; j < m; j++) {
      mat[n-1][j] = cRandom() <= prob ? 1 : 0;
    }
  }
  for (i = threadData[tid].i0; i < threadData[tid].i1; i++) {
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
static inline double cRandom() {
  // https://stackoverflow.com/questions/6218399/how-to-generate-a-random-number-between-0-and-1
  return (double) rand() / (double) RAND_MAX;
}



/*
 * Function printMatrix
 * --------------------
 *  Print matrix to console
 *
 *  mat: pointer to the first element of the matrix
 *  nRows: number of rows of the matrix
 *  nCols: number of columns of the matrix
 */
void printMatrix(char** restrict mat, const int nRows, const int nCols) {
  int i, j;
  for (i = 0; i < nRows; i++) {
    printf("[ ");
    for (j = 0; j < nCols; j++) {
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
