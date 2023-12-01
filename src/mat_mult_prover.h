#ifndef MAT_MULT_PROVER_H
#define MAT_MULT_PROVER_H

#include "graph.h"
#include "lagrange.h"
#include "matrix.h"
#include "modulo.h"

typedef struct {
  Vector *chosen_values;
  Vector *values_a;
  Vector *values_b;
  Matrix *matrix_a;
  Matrix *matrix_b;
  Vector *indexes_1;
  Vector *indexes_2;
  long base;
  unsigned int power;
  unsigned int last_round;
  unsigned int round;
} MatMultProver;

typedef struct {
  long result;
  Vector *values;
} MatMultProofStartResult;

extern MatMultProver *create_mat_mult_prover(Matrix *matrix_a, Matrix *matrix_b,
                                             Vector *indexes_1,
                                             Vector *indexes_2, long base);
extern MatMultProofStartResult mat_mult_prover_start(MatMultProver *prover,
                                                     Matrix *matrix_product);
extern Vector *mat_mult_prover_execute_next_round(MatMultProver *prover,
                                                  long chosen_value);
extern void destroy_mat_mult_prover(MatMultProver *prover);

#endif