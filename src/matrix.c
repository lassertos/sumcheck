#include "matrix.h"
#include <gsl/gsl_blas.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Matrix *create_matrix(unsigned int rows, unsigned int columns) {
  Matrix *matrix = malloc(sizeof(Matrix));
  matrix->values = malloc(sizeof(long *) * rows);

  matrix->rows = rows;
  matrix->columns = columns;

  for (int i = 0; i < rows; i++) {
    matrix->values[i] = malloc(sizeof(long) * columns);
    for (int j = 0; j < columns; j++) {
      matrix->values[i][j] = 0;
    }
  }

  return matrix;
}

void destroy_matrix(Matrix *matrix) {
  for (int i = 0; i < matrix->rows; i++) {
    free(matrix->values[i]);
  }
  free(matrix->values);
  free(matrix);
}

Matrix *multiply_matrices(Matrix *matrix_a, Matrix *matrix_b) {
  if (matrix_a->columns != matrix_b->rows)
    return NULL;

  Matrix *result_matrix = create_matrix(matrix_a->rows, matrix_b->columns);

  for (unsigned int i = 0; i < matrix_a->rows; i++) {
    for (unsigned int j = 0; j < matrix_b->columns; j++) {
      for (unsigned int k = 0; k < matrix_a->columns; k++) {
        if (matrix_a->values[i][k] && matrix_b->values[k][j]) {
          result_matrix->values[i][j] +=
              matrix_a->values[i][k] * matrix_b->values[k][j];
        }
      }
    }
  }

  return result_matrix;
}

Matrix *square_adjacency_matrix(Matrix *adjacency_matrix) {
  if (adjacency_matrix->rows != adjacency_matrix->columns)
    return NULL;

  unsigned int size = adjacency_matrix->rows * adjacency_matrix->columns;

  double *preparedA = malloc(sizeof(double) * size);
  double *preparedC = malloc(sizeof(double) * size);

  for (unsigned int i = 0; i < adjacency_matrix->rows; i++) {
    for (unsigned int j = 0; j < adjacency_matrix->columns; j++) {
      preparedA[i * adjacency_matrix->rows + j] =
          (double)(adjacency_matrix->values[i][j]);
      preparedC[i * adjacency_matrix->rows + j] = 0.0;
    }
  }

  Matrix *squared_adjacency_matrix =
      create_matrix(adjacency_matrix->rows, adjacency_matrix->columns);

  gsl_matrix_view A = gsl_matrix_view_array(preparedA, adjacency_matrix->rows,
                                            adjacency_matrix->columns);
  gsl_matrix_view C = gsl_matrix_view_array(preparedC, adjacency_matrix->rows,
                                            adjacency_matrix->columns);

  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, &A.matrix, &A.matrix, 0.0,
                 &C.matrix);

  for (unsigned int i = 0; i < adjacency_matrix->rows; i++) {
    for (unsigned int j = 0; j < adjacency_matrix->columns; j++) {
      squared_adjacency_matrix->values[i][j] =
          (long)preparedC[i * adjacency_matrix->rows + j];
    }
  }

  free(preparedA);
  free(preparedC);

  return squared_adjacency_matrix;
}

Vector *matrix_to_function(Matrix *matrix) {
  unsigned int max = 0;
  if (matrix->rows > matrix->columns) {
    max = matrix->rows;
  } else {
    max = matrix->columns;
  }

  unsigned int new_size = pow(2.0, ceil(log2(max)));
  Vector *function = create_vector(new_size * new_size);

  for (unsigned int i = 0; i < new_size; i++) {
    for (unsigned int j = 0; j < new_size; j++) {
      if (i < matrix->rows && j < matrix->columns) {
        function->values[new_size * i + j] = matrix->values[i][j];
      } else {
        function->values[new_size * i + j] = 0;
      }
    }
  }

  return function;
}

void print_matrix(Matrix *matrix) {
  if (!matrix)
    printf("NULL");

  printf("rows: %d\n", matrix->rows);
  printf("columns: %d\n", matrix->columns);
  printf("Matrix:\n\n");

  for (int i = 0; i < matrix->rows; i++) {
    for (int j = 0; j < matrix->columns; j++) {
      printf("%ld ", matrix->values[i][j]);
    }
    printf("\n");
  }
}
