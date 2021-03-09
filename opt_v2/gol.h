#ifndef GOL_H
#define GOL_H

#define ELEMS 4

typedef struct block {
  char data[ELEMS*ELEMS];
  struct block* topleft;
  struct block* top;
  struct block* topright;
  struct block* left;
  struct block* right;
  struct block* botleft;
  struct block* bot;
  struct block* botright;
  char active;  // Whether block has changed or not
} block_t;

void evolve(const int n, const int m, const int nSteps);

#endif
