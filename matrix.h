#ifndef MATRIX_MATRIX_H
#define MATRIX_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

struct Matrix_data;
typedef struct Matrix_data {
  int row;
  int col;
  double ** data;
} Matrix;

Matrix * createMatrix(int m, int n);

#ifdef __cplusplus
}
#endif

#endif // MATRIX_MATRIX_H
