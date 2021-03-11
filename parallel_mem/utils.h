#ifndef UTILS_H
#define UTILS_H

#include "gol.h"

void printMatrix(char** restrict mat, const int nRows, const int nCols);
char** allocateMatrix(const int nRows, const int nCols);
void freeMatrix(char** restrict mat, const int nRows, const int nCols);
void createInitialState(char** restrict mat, const int n, const int m,
                        const double prob, const int nThreads,
                        tdata_t* restrict threadData);
double get_wall_seconds();

#endif
