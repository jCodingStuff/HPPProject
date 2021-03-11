#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include "gol.h"
#include "utils.h"



// Static function declarations
static inline void decide(const char alive, char** restrict future, const int i,
                          const int j, const char field);


char** restrict state; // State at even times (0, 2, 4, etc.)
char** restrict other; // State at odd times (1, 3, 5, etc.)
tdata_t* restrict threadData;  // Data for the threads to operate

pthread_t* restrict threads;
pthread_mutex_t* restrict locks;  // Signals to indicate work is done
pthread_cond_t* restrict signals;  // Signals to indicate work is done
char* restrict doneState;
char* restrict doneOther;


int n, m;  // Number of rows and columns, respectively
int nSteps;  // Number of time steps
int nThreads;  // Number of threads


int main(int argc, char const *argv[]) {

  int i;

  // Take initial time
  double t1 = get_wall_seconds();

  // Check that arguments are provided
  if (argc != 8) {
    printf("Usage: %s n m prob nSteps seed nThreads debug\n", argv[0]);
    return -1;
  }

  // Parse arguments
  n = atoi(argv[1]);
  m = atoi(argv[2]);
  const double prob = atof(argv[3]);
  nSteps = atoi(argv[4]);
  const int seed = atoi(argv[5]);
  nThreads = atoi(argv[6]);
  const int debug = atoi(argv[7]);
  // printf("%d %d %lf %d\n", n, m, prob, nSteps);

  // Check that arguments are valid
  if (n <= 0 || m <= 0 || nSteps <= 0 || prob < 0 || prob > 1 || nThreads <= 0) {
    printf("Usage:\n  n, m, nSteps and nThreads must be positive integers\n  prob must be in range [0, 1]\n");
    return -1;
  }

  // Initialize arbitrary seed for random numbers (or not!)
  if (seed < 0) {
    srand(time(NULL));
  } else {
    srand((unsigned int) seed);
  }

  // Initialize data structures
  state = allocateMatrix(n, m);
  other = allocateMatrix(n, m);

  // Create initial state
  createInitialState(state, n, m, prob);

  // Create row work indicators
  doneState = (char*) malloc(n * sizeof(char));
  doneOther = (char*) calloc(n, sizeof(char));  // Want to initialize as 0s
  memset(doneState, 1, n*sizeof(char));

  // Initialize synchronization variables
  locks = (pthread_mutex_t*) malloc(nThreads*sizeof(pthread_mutex_t));
  signals = (pthread_cond_t*) malloc(nThreads*sizeof(pthread_cond_t));
  for (i = 0; i < nThreads; i++) {
    pthread_cond_init(signals + i, NULL);
    pthread_mutex_init(locks + i, NULL);
  }

  // Print initial state
  if (debug) {
    printf("Initial state:\n");
    printMatrix(state, n, m);
  }

  // Prepare data for threads and evolve the system
  threads = (pthread_t*) malloc(nThreads*sizeof(pthread_t));
  threadData = (tdata_t*) malloc(nThreads*sizeof(tdata_t));
  // Distribute work evenly for (rows 2 to n-2)
  const int elePerThread = (n-2)/nThreads;
  const int remainder = (n-2)%nThreads;
  for (i = 0; i < remainder; i++) {
    threadData[i].i0 = i*(elePerThread+1)+1;
    threadData[i].i1 = (i+1)*(elePerThread+1)+1;
    int tid = i;
    pthread_create(threads + i, NULL, evolveThread, (void*) (&tid));
  }
  for (i = remainder; i < nThreads; i++) {
    threadData[i].i0 = remainder + i*elePerThread + 1;
    threadData[i].i1 = remainder + (i+1)*elePerThread + 1;
    int tid = i;
    pthread_create(threads + i, NULL, evolveThread, (void*) (&tid));
  }

  // Print final state
  if (debug) {
    printf("Final state:\n");
    printMatrix(state, n, m);
  }

  // Destroy synchronization variables
  for (i = 0; i < nThreads; i++) {
    pthread_cond_destroy(signals + i);
    pthread_mutex_destroy(locks + i);
  }

  // Free data structures
  freeMatrix(state, n, m);
  freeMatrix(other, n, m);
  free(threadData);
  free(threads);
  free(locks);
  free(signals);
  free(doneState);
  free(doneOther);

  // Print time it took to run the code
  t1 = get_wall_seconds() - t1;
  if (debug) {
    printf("Execution took %lf seconds\n", t1);
  } else {
    printf("%lf\n", t1);
  }

  return 0;
}


/*
 * Function evolveThread
 * ---------------
 *  Evolve the game state for a given number of iterations (threaded)
 *
 *  n: number of rows of the matrix
 *  m: number of columns of the matrix
 *  nSteps: number of iterations (assumed to be even)
 *  nThreads: number of threads
 *  threadData: pointer to the first element of the array containing thread data
 */
void* evolveThread(void* arg) {
  int tid = *((int*) arg);
  int k, i, j;
  char field;

  if (tid == 0) {
    for (k = 0; k < nSteps; k+=2) {

      // FIRST ITERATION
      {
      // First row (i=0)
        // Set row to undone
        doneOther = 0;
        // Verify that last row was already done previosly
        pthread_mutex_lock(locks + n-1);
        while (!doneState[n-1]) {
          pthread_cond_wait(signals + n-1, locks + n-1);
        }
        pthread_mutex_unlock(locks + n-1);
        // First column (j=0)
        field = state[n-1][m-1] + state[n-1][0] + state[n-1][1]
                    + state[0][m-1] + state[0][0] + state[0][1]
                    + state[1][m-1] + state[1][0] + state[1][1];
        decide(state[0][0], other, 0, 0, field);
        // Other columns (j=1 to m-2)
        for (j=1; j <= m - 2; j++) {
          field = state[n-1][j-1] + state[n-1][j] + state[n-1][j+1]
                      + state[0][j-1] + state[0][j] + state[0][j+1]
                      + state[1][j-1] + state[1][j] + state[1][j+1];
          decide(state[0][j], other, 0, j, field);
        }
        // Last column (j=m-1)
        field = state[n-1][m-2] + state[n-1][m-1] + state[n-1][0]
                    + state[0][m-2] + state[0][m-1] + state[0][0]
                    + state[1][m-2] + state[1][m-1] + state[1][0];
        decide(state[0][m-1], other, 0, m-1, field);
        // Set row to done for future and undone for present
        doneOther = 1;
        pthread_cond_broadcast(signals);

      // Other rows (i=1 to n-2) - Note the separation for each thread
      for (i = threadData[tid].i0; i < threadData[tid].i1; i++) {
        // First column (j=0)
        field = state[i-1][m-1] + state[i-1][0] + state[i-1][1]
                    + state[i][m-1] + state[i][0] + state[i][1]
                    + state[i+1][m-1] + state[i+1][0] + state[i+1][1];
        decide(state[i][0], other, i, 0, field);
        // Other columns (j=1 to m-2)
        for (j=1; j <= m - 2; j++) {
          field = state[i-1][j-1] + state[i-1][j] + state[i-1][j+1]
                      + state[i][j-1] + state[i][j] + state[i][j+1]
                      + state[i+1][j-1] + state[i+1][j] + state[i+1][j+1];
          decide(state[i][j], other, i, j, field);
        }
        // Last column (j=m-1)
        field = state[i-1][m-2] + state[i-1][m-1] + state[i-1][0]
                    + state[i][m-2] + state[i][m-1] + state[i][0]
                    + state[i+1][m-2] + state[i+1][m-1] + state[i+1][0];
        decide(state[i][m-1], other, i, m-1, field);
      }

      }

      // SECOND ITERATION
      {
      // First row (i=0)
        // First column (j=0)
        field = other[n-1][m-1] + other[n-1][0] + other[n-1][1]
                    + other[0][m-1] + other[0][0] + other[0][1]
                    + other[1][m-1] + other[1][0] + other[1][1];
        decide(other[0][0], state, 0, 0, field);
        // Other columns (j=1 to m-2)
        for (j=1; j <= m - 2; j++) {
          field = other[n-1][j-1] + other[n-1][j] + other[n-1][j+1]
                      + other[0][j-1] + other[0][j] + other[0][j+1]
                      + other[1][j-1] + other[1][j] + other[1][j+1];
          decide(other[0][j], state, 0, j, field);
        }
        // Last column (j=m-1)
        field = other[n-1][m-2] + other[n-1][m-1] + other[n-1][0]
                    + other[0][m-2] + other[0][m-1] + other[0][0]
                    + other[1][m-2] + other[1][m-1] + other[1][0];
        decide(other[0][m-1], state, 0, m-1, field);

      // Other rows (i=1 to n-2) - Note the separation for each thread
      for (i = threadData[tid].i0; i < threadData[tid].i1; i++) {
        // First column (j=0)
        field = other[i-1][m-1] + other[i-1][0] + other[i-1][1]
                    + other[i][m-1] + other[i][0] + other[i][1]
                    + other[i+1][m-1] + other[i+1][0] + other[i+1][1];
        decide(other[i][0], state, i, 0, field);
        // Other columns (j=1 to m-2)
        for (j=1; j <= m - 2; j++) {
          field = other[i-1][j-1] + other[i-1][j] + other[i-1][j+1]
                      + other[i][j-1] + other[i][j] + other[i][j+1]
                      + other[i+1][j-1] + other[i+1][j] + other[i+1][j+1];
          decide(other[i][j], state, i, j, field);
        }
        // Last column (j=m-1)
        field = other[i-1][m-2] + other[i-1][m-1] + other[i-1][0]
                    + other[i][m-2] + other[i][m-1] + other[i][0]
                    + other[i+1][m-2] + other[i+1][m-1] + other[i+1][0];
        decide(other[i][m-1], state, i, m-1, field);
      }

      }

    }
  } else if (tid == nThreads-1) {
    for (k = 0; k < nSteps; k+=2) {

      // FIRST ITERATION
      {

      // Other rows (i=1 to n-2) - Note the separation for each thread
      for (i = threadData[tid].i0; i < threadData[tid].i1; i++) {
        // First column (j=0)
        field = state[i-1][m-1] + state[i-1][0] + state[i-1][1]
                    + state[i][m-1] + state[i][0] + state[i][1]
                    + state[i+1][m-1] + state[i+1][0] + state[i+1][1];
        decide(state[i][0], other, i, 0, field);
        // Other columns (j=1 to m-2)
        for (j=1; j <= m - 2; j++) {
          field = state[i-1][j-1] + state[i-1][j] + state[i-1][j+1]
                      + state[i][j-1] + state[i][j] + state[i][j+1]
                      + state[i+1][j-1] + state[i+1][j] + state[i+1][j+1];
          decide(state[i][j], other, i, j, field);
        }
        // Last column (j=m-1)
        field = state[i-1][m-2] + state[i-1][m-1] + state[i-1][0]
                    + state[i][m-2] + state[i][m-1] + state[i][0]
                    + state[i+1][m-2] + state[i+1][m-1] + state[i+1][0];
        decide(state[i][m-1], other, i, m-1, field);
      }

      // Last row (i=n-1)
        // First column (j=0)
        field = state[n-2][m-1] + state[n-2][0] + state[n-2][1]
                    + state[n-1][m-1] + state[n-1][0] + state[n-1][1]
                    + state[0][m-1] + state[0][0] + state[0][1];
        decide(state[n-1][0], other, n-1, 0, field);
        // Other columns (j=1 to m-2)
        for (j=1; j <= m - 2; j++) {
          field = state[n-2][j-1] + state[n-2][j] + state[n-2][j+1]
                      + state[n-1][j-1] + state[n-1][j]  + state[n-1][j+1]
                      + state[0][j-1] + state[0][j] + state[0][j+1];
          decide(state[n-1][j], other, n-1, j, field);
        }
        // Last column (j=m-1)
        field = state[n-2][m-2] + state[n-2][m-1] + state[n-2][0]
                    + state[n-1][m-2] + state[n-1][m-1] + state[n-1][0]
                    + state[0][m-2] + state[0][m-1] + state[0][0];
        decide(state[n-1][m-1], other, n-1, m-1, field);

      }

      // SECOND ITERATION
      {

      // Other rows (i=1 to n-2) - Note the separation for each thread
      for (i = threadData[tid].i0; i < threadData[tid].i1; i++) {
        // First column (j=0)
        field = other[i-1][m-1] + other[i-1][0] + other[i-1][1]
                    + other[i][m-1] + other[i][0] + other[i][1]
                    + other[i+1][m-1] + other[i+1][0] + other[i+1][1];
        decide(other[i][0], state, i, 0, field);
        // Other columns (j=1 to m-2)
        for (j=1; j <= m - 2; j++) {
          field = other[i-1][j-1] + other[i-1][j] + other[i-1][j+1]
                      + other[i][j-1] + other[i][j] + other[i][j+1]
                      + other[i+1][j-1] + other[i+1][j] + other[i+1][j+1];
          decide(other[i][j], state, i, j, field);
        }
        // Last column (j=m-1)
        field = other[i-1][m-2] + other[i-1][m-1] + other[i-1][0]
                    + other[i][m-2] + other[i][m-1] + other[i][0]
                    + other[i+1][m-2] + other[i+1][m-1] + other[i+1][0];
        decide(other[i][m-1], state, i, m-1, field);
      }

      // Last row (i=n-1)
        // First column (j=0)
        field = other[n-2][m-1] + other[n-2][0] + other[n-2][1]
                    + other[n-1][m-1] + other[n-1][0] + other[n-1][1]
                    + other[0][m-1] + other[0][0] + other[0][1];
        decide(other[n-1][0], state, n-1, 0, field);
        // Other columns (j=1 to m-2)
        for (j=1; j <= m - 2; j++) {
          field = other[n-2][j-1] + other[n-2][j] + other[n-2][j+1]
                      + other[n-1][j-1] + other[n-1][j]  + other[n-1][j+1]
                      + other[0][j-1] + other[0][j] + other[0][j+1];
          decide(other[n-1][j], state, n-1, j, field);
        }
        // Last column (j=m-1)
        field = other[n-2][m-2] + other[n-2][m-1] + other[n-2][0]
                    + other[n-1][m-2] + other[n-1][m-1] + other[n-1][0]
                    + other[0][m-2] + other[0][m-1] + other[0][0];
        decide(other[n-1][m-1], state, n-1, m-1, field);

      }

    }
  } else {
    for (k = 0; k < nSteps; k+=2) {

      // FIRST ITERATION
      {

      // Other rows (i=1 to n-2) - Note the separation for each thread
      for (i = threadData[tid].i0; i < threadData[tid].i1; i++) {
        // First column (j=0)
        field = state[i-1][m-1] + state[i-1][0] + state[i-1][1]
                    + state[i][m-1] + state[i][0] + state[i][1]
                    + state[i+1][m-1] + state[i+1][0] + state[i+1][1];
        decide(state[i][0], other, i, 0, field);
        // Other columns (j=1 to m-2)
        for (j=1; j <= m - 2; j++) {
          field = state[i-1][j-1] + state[i-1][j] + state[i-1][j+1]
                      + state[i][j-1] + state[i][j] + state[i][j+1]
                      + state[i+1][j-1] + state[i+1][j] + state[i+1][j+1];
          decide(state[i][j], other, i, j, field);
        }
        // Last column (j=m-1)
        field = state[i-1][m-2] + state[i-1][m-1] + state[i-1][0]
                    + state[i][m-2] + state[i][m-1] + state[i][0]
                    + state[i+1][m-2] + state[i+1][m-1] + state[i+1][0];
        decide(state[i][m-1], other, i, m-1, field);
      }

      }

      // SECOND ITERATION
      {

      // Other rows (i=1 to n-2) - Note the separation for each thread
      for (i = threadData[tid].i0; i < threadData[tid].i1; i++) {
        // First column (j=0)
        field = other[i-1][m-1] + other[i-1][0] + other[i-1][1]
                    + other[i][m-1] + other[i][0] + other[i][1]
                    + other[i+1][m-1] + other[i+1][0] + other[i+1][1];
        decide(other[i][0], state, i, 0, field);
        // Other columns (j=1 to m-2)
        for (j=1; j <= m - 2; j++) {
          field = other[i-1][j-1] + other[i-1][j] + other[i-1][j+1]
                      + other[i][j-1] + other[i][j] + other[i][j+1]
                      + other[i+1][j-1] + other[i+1][j] + other[i+1][j+1];
          decide(other[i][j], state, i, j, field);
        }
        // Last column (j=m-1)
        field = other[i-1][m-2] + other[i-1][m-1] + other[i-1][0]
                    + other[i][m-2] + other[i][m-1] + other[i][0]
                    + other[i+1][m-2] + other[i+1][m-1] + other[i+1][0];
        decide(other[i][m-1], state, i, m-1, field);
      }

      }

    }
  }

  return NULL;

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
