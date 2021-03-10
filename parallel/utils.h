#ifndef UTILS_H
#define UTILS_H

void printMatrix(char** restrict mat, const int nRows, const int nCols);
char** allocateMatrix(const int nRows, const int nCols);
void freeMatrix(char** restrict mat, const int nRows, const int nCols);
void createInitialState(char** restrict mat, const int nRows, const int nCols,
                        const double prob, const int nThreads);
double get_wall_seconds();

#endif
