#ifndef GOL_H
#define GOL_H

/*
 * Structure tdata
 * ---------------
 *  Contains data for threads to operate
 *
 *  i0: starting index (inclusive)
 *  i1: ending index (exclusive)
 */
typedef struct tdata {
  int i0;  // Inclusive
  int i1;  // Exclusive
} tdata_t;

void evolve(const int n, const int m, const int nSteps, const int nThreads,
            tdata_t* restrict threadData);

#endif
