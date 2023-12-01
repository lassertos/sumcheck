#include "mat_mult_prover.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

MatMultProver *create_mat_mult_prover(Matrix *matrix_a, Matrix *matrix_b,
                                      Vector *indexes_1, Vector *indexes_2,
                                      long base) {
  MatMultProver *prover = malloc(sizeof(MatMultProver));

  prover->base = base;

  prover->matrix_a = matrix_a;
  prover->matrix_b = matrix_b;

  prover->power = (unsigned int)(ceil(log2(prover->matrix_a->rows)));

  prover->indexes_1 = indexes_1;
  prover->indexes_2 = indexes_2;

  prover->round = 0;
  prover->last_round = prover->power - 1;

  prover->chosen_values = create_vector(prover->power - 1);

  return prover;
}

MatMultProofStartResult mat_mult_prover_start(MatMultProver *prover,
                                              Matrix *matrix_product) {
  prover->round = 0;
  unsigned long size = 1UL << (prover->power);

  prover->values_a = create_vector(size);
  prover->values_b = create_vector(size);

  Matrix *result_matrix =
      matrix_product != NULL
          ? matrix_product
          : multiply_matrices(prover->matrix_a, prover->matrix_b);

  Vector *result_matrix_function = matrix_to_function(result_matrix);

  Matrix *prepared = create_matrix(2, prover->matrix_a->rows);
  for (unsigned int i = 0; i < prover->matrix_a->rows; i++) {
    BinaryVector *w = create_binary_vector_from(prover->power, i);
    prepared->values[0][i] =
        evaluate_lagrange_basis_polynomial(w, prover->indexes_1, prover->base);
    prepared->values[1][i] =
        evaluate_lagrange_basis_polynomial(w, prover->indexes_2, prover->base);
    free(w);
  }

  for (unsigned int i = 0; i < prover->matrix_a->rows; i++) {
    for (unsigned int j = 0; j < prover->matrix_a->columns; j++) {
      if (prover->matrix_a->values[i][j]) {
        __int128_t tmp = prover->values_a->values[j] + prepared->values[0][i];
        prover->values_a->values[j] = modulo(tmp, prover->base);
      }

      if (prover->matrix_b->values[i][j]) {
        __int128_t tmp = prover->values_b->values[i] + prepared->values[1][j];
        prover->values_b->values[i] = modulo(tmp, prover->base);
      }
    }
  }

  Vector *vector = create_vector(prover->power * 2);
  for (unsigned int i = 0; i < prover->power; i++) {
    vector->values[i] = prover->indexes_1->values[i];
    vector->values[i + prover->power] = prover->indexes_2->values[i];
  }

  long result = evaluate_multilinear_extension(result_matrix_function, vector,
                                               prover->base);

  // TODO: free result_matrix, result_matrix_function, prepared and vector

  MatMultProofStartResult start_result = {
      .result = result,
      .values = mat_mult_prover_execute_next_round(prover, -1)};

  return start_result;
}

Vector *mat_mult_prover_execute_next_round(MatMultProver *prover,
                                           long chosen_value) {
  prover->round++;
  Vector *new_values = create_vector(3);

  if (chosen_value >= 0) {
    prover->chosen_values->values[prover->round - 2] = chosen_value;
    unsigned int remaining_spaces = prover->power - (prover->round - 1);
    unsigned long size = 1UL << remaining_spaces;

    Vector *new_values_a = create_vector(size);
    Vector *new_values_b = create_vector(size);

    for (unsigned long i = 0; i < size; i++) {
      __int128_t tmp = (__int128_t)(1 - chosen_value) *
                       (__int128_t)prover->values_a->values[i];
      tmp = (__int128_t)modulo(tmp, prover->base) +
            ((__int128_t)chosen_value *
             (__int128_t)prover->values_a->values[i + size]);
      new_values_a->values[i] = modulo(tmp, prover->base);

      tmp = (__int128_t)(1 - chosen_value) *
            (__int128_t)prover->values_b->values[i];
      tmp = (__int128_t)modulo(tmp, prover->base) +
            ((__int128_t)chosen_value *
             (__int128_t)prover->values_b->values[i + size]);
      new_values_b->values[i] = modulo(tmp, prover->base);
    }

    free(prover->values_a);
    free(prover->values_b);

    prover->values_a = new_values_a;
    prover->values_b = new_values_b;

    for (unsigned long i = 0; i < size / 2; i++) {
      __int128_t tmp = new_values->values[0];
      tmp += ((__int128_t)prover->values_a->values[i] *
              (__int128_t)prover->values_b->values[i]);
      new_values->values[0] = modulo(tmp, prover->base);
    }

    for (unsigned long i = size / 2; i < size; i++) {
      __int128_t tmp = new_values->values[1];
      tmp += ((__int128_t)prover->values_a->values[i] *
              (__int128_t)prover->values_b->values[i]);
      new_values->values[1] = modulo(tmp, prover->base);

      tmp = new_values->values[2];
      __int128_t tmp_1, tmp_2;
      tmp_1 = ((__int128_t)2 * (__int128_t)prover->values_a->values[i]) -
              (__int128_t)prover->values_a->values[i - size / 2];
      tmp_2 = ((__int128_t)2 * (__int128_t)prover->values_b->values[i]) -
              (__int128_t)prover->values_b->values[i - size / 2];
      tmp += (__int128_t)modulo(tmp_1, prover->base) *
             (__int128_t)modulo(tmp_2, prover->base);
      new_values->values[2] = modulo(tmp, prover->base);
    }
  } else {
    unsigned int remaining_spaces = prover->power - (prover->round - 1);
    unsigned long size = 1UL << remaining_spaces;

    for (unsigned long i = 0; i < size / 2; i++) {
      __int128_t tmp = new_values->values[0];
      tmp += ((__int128_t)prover->values_a->values[i] *
              (__int128_t)prover->values_b->values[i]);
      new_values->values[0] = modulo(tmp, prover->base);
    }

    for (unsigned long i = size / 2; i < size; i++) {
      __int128_t tmp = new_values->values[1];
      tmp += ((__int128_t)prover->values_a->values[i] *
              (__int128_t)prover->values_b->values[i]);
      new_values->values[1] = modulo(tmp, prover->base);

      tmp = new_values->values[2];
      __int128_t tmp_1, tmp_2;
      tmp_1 = ((__int128_t)2 * (__int128_t)prover->values_a->values[i]) -
              prover->values_a->values[i - size / 2];
      tmp_2 = ((__int128_t)2 * (__int128_t)prover->values_b->values[i]) -
              prover->values_b->values[i - size / 2];
      tmp += (__int128_t)modulo(tmp_1, prover->base) *
             (__int128_t)modulo(tmp_2, prover->base);
      new_values->values[2] = modulo(tmp, prover->base);
    }
  }

  return new_values;
}