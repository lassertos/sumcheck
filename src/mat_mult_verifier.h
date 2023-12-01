#ifndef MAT_MULT_VERIFIER_H
#define MAT_MULT_VERIFIER_H

#include "lagrange.h"
#include "matrix.h"
#include "modulo.h"
#include "vector.h"

typedef struct {
  Vector *claimed_results;
  Vector *chosen_values;
  Matrix *matrix_a;
  Matrix *matrix_b;
  Vector *indexes_1;
  Vector *indexes_2;
  long base;
  unsigned int power;
  unsigned int last_round;
  unsigned int round;
} MatMultVerifier;

typedef struct {
  unsigned char result;
  unsigned char finished;
  long chosen_value;
} MatMultVerificationResult;

extern MatMultVerifier *create_mat_mult_verifier(Matrix *matrix_a,
                                                 Matrix *matrix_b,
                                                 Vector *indexes_1,
                                                 Vector *indexes_2, long base);
extern MatMultVerificationResult
mat_mult_verifier_start_validation(MatMultVerifier *verifier,
                                   long claimed_result, Vector *values);
extern MatMultVerificationResult
mat_mult_verifier_execute_next_round(MatMultVerifier *verifier, Vector *values);
extern MatMultVerificationResult execute_last_round(MatMultVerifier *verifier);

#endif