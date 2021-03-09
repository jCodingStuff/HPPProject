#ifndef UTILS_H
#define UTILS_H

void printMatrix(block_t* restrict mat, const int nb, const int mb);
char* allocateMatrix(const int nRows, const int nCols);
block_t* initBlocks(const int nb, const int mb);
void createInitialState(block_t* restrict mat, const int nb, const int mb,
                        const double prob);
double get_wall_seconds();

#endif
