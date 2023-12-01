#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
  unsigned int rows;
  unsigned int columns;
  long **values;
} Matrix;

extern Matrix *create_matrix(unsigned int, unsigned int);
extern void print_matrix(Matrix *);
extern Vector *matrix_to_function(Matrix *);
extern Matrix *square_adjacency_matrix(Matrix *);
extern Matrix *multiply_matrices(Matrix *, Matrix *);
extern void destroy_matrix(Matrix *matrix);

#endif