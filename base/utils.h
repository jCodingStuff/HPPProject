#ifndef UTILS_H
#define UTILS_H

void printMatrix(int** mat, int n, int m);
int** allocateMatrix(int n, int m);
void freeMatrix(int** mat, int n, int m);
void createInitialState(int** mat, int n, int m, double prob);
double get_wall_seconds();
double cRandom();

#endif
