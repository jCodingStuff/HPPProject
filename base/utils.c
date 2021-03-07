#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>


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
 *  nRows: number of rows of the matrix
 *  nCols: number of columns of the matrix
 *  prob: probability of a cell being alive
 */
void createInitialState(char** restrict mat, const int nRows, const int nCols,
                        const double prob) {
  int i, j;
  for (i = 0; i < nRows; i++) {
    for (j = 0; j < nCols; j++) {
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
