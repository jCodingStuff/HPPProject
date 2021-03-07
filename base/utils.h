#ifndef UTILS_H
#define UTILS_H

void printMatrix(char** mat, const int nRows, const int nCols);
char** allocateMatrix(const int nRows, const int nCols);
void freeMatrix(char** mat, const int nRows, const int nCols);
void createInitialState(char** mat, const int nRows, const int nCols,
                        const double prob);
double get_wall_seconds();

#endif
