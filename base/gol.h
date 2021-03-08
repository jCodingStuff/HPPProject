#ifndef GOL_H
#define GOL_H

void evolve(int n, int m, int nSteps);
void decide(int alive, int** future, int i, int j, int neighbors);

#endif
