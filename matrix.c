#include <stdlib.h>
#include "matrix.h"

Matrix * createMatrix(int m, int n) {
  Matrix * mat = malloc(sizeof(Matrix));
  mat->row = m;
  mat->col = n;
  mat->data = malloc(sizeof(double*) * m);
  double *pool = malloc(sizeof(double) * m * n);
  for (int i = 0; i < m; ++i)
    mat->data[i] = pool + i * n;
  return mat;
}
