#include "mat_mult_verifier.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

MatMultVerifier *create_mat_mult_verifier(Matrix *matrix_a, Matrix *matrix_b,
                                          Vector *indexes_1, Vector *indexes_2,
                                          long base) {
  MatMultVerifier *verifier = malloc(sizeof(MatMultVerifier));

  verifier->base = base;

  verifier->matrix_a = matrix_a;
  verifier->matrix_b = matrix_b;

  verifier->power = (unsigned int)(ceil(log2(verifier->matrix_a->rows)));

  verifier->indexes_1 = indexes_1;
  verifier->indexes_2 = indexes_2;

  verifier->round = 0;
  verifier->last_round = verifier->power - 1;

  verifier->chosen_values = create_vector(verifier->power);
  verifier->claimed_results = create_vector(verifier->power + 1);

  return verifier;
}

MatMultVerificationResult
mat_mult_verifier_start_validation(MatMultVerifier *verifier,
                                   long claimed_result, Vector *values) {
  verifier->claimed_results->values[0] = claimed_result;
  verifier->round = 0;

  return mat_mult_verifier_execute_next_round(verifier, values);
}

MatMultVerificationResult
mat_mult_verifier_execute_next_round(MatMultVerifier *verifier,
                                     Vector *values) {
  long eval_0 =
      evaluate_univariate_lagrange_interpolation(values, 0, verifier->base);
  long eval_1 =
      evaluate_univariate_lagrange_interpolation(values, 1, verifier->base);

  __int128_t tmp = eval_0 + eval_1;

  if (modulo(tmp, verifier->base) !=
      verifier->claimed_results->values[verifier->round]) {
    verifier->round++;
    MatMultVerificationResult validation_result = {.result = 0, .finished = 1};
    return validation_result;
  }

  long chosen_value =
      (long)floor(((double)rand() / RAND_MAX) * (verifier->base - 1));
  verifier->chosen_values->values[verifier->round] = chosen_value;
  verifier->claimed_results->values[verifier->round + 1] =
      evaluate_univariate_lagrange_interpolation(values, chosen_value,
                                                 verifier->base);

  if (verifier->round == verifier->last_round)
    return execute_last_round(verifier);

  verifier->round++;

  MatMultVerificationResult validation_result = {.result = 1,
                                                 .chosen_value = chosen_value};

  return validation_result;
}

MatMultVerificationResult execute_last_round(MatMultVerifier *verifier) {
  verifier->round++;
  Vector *matrix_function_a = matrix_to_function(verifier->matrix_a);
  Vector *matrix_function_b = matrix_to_function(verifier->matrix_b);

  Vector *vector_a = create_vector(verifier->power * 2);
  Vector *vector_b = create_vector(verifier->power * 2);

  for (int i = 0; i < verifier->power; i++) {
    vector_a->values[i] = verifier->indexes_1->values[i];
    vector_a->values[i + verifier->power] = verifier->chosen_values->values[i];
    vector_b->values[i] = verifier->chosen_values->values[i];
    vector_b->values[i + verifier->power] = verifier->indexes_2->values[i];
  }

  long value_a = evaluate_multilinear_extension(matrix_function_a, vector_a,
                                                verifier->base);
  long value_b = evaluate_multilinear_extension(matrix_function_b, vector_b,
                                                verifier->base);

  __int128_t tmp = (__int128_t)value_a * (__int128_t)value_b;
  long result = modulo(tmp, verifier->base);

  MatMultVerificationResult validation_result = {
      .result = result == verifier->claimed_results->values[verifier->round],
      .finished = 1};

  return validation_result;
}