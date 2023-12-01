#include "counting_triangles_verifier.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

CountingTrianglesVerifier *
create_counting_triangles_verifier(Graph *graph,
                                   CountingTrianglesProver *prover, long base) {
  clock_t start = clock();

  CountingTrianglesVerifier *verifier =
      malloc(sizeof(CountingTrianglesVerifier));

  verifier->base = base;
  verifier->prover = prover;

  verifier->adjacency_matrix = get_adjacency_matrix(graph);
  verifier->power =
      (unsigned int)(ceil(log2(verifier->adjacency_matrix->rows)));

  verifier->round = 0;
  verifier->last_round = (verifier->power * 2) - 1;

  verifier->chosen_values = create_vector(verifier->power * 2);
  verifier->claimed_results = create_vector(verifier->power * 2 + 1);

  verifier->time_taken = ((clock() - start) / (CLOCKS_PER_SEC / 1000));

  return verifier;
}

CountingTrianglesVerificationResult
counting_triangles_verifier_start_validation(
    CountingTrianglesVerifier *verifier, long claimed_result, Vector *values) {
  clock_t start = clock();

  verifier->claimed_results->values[0] = claimed_result;
  verifier->round = 0;

  verifier->time_taken += ((clock() - start) / (CLOCKS_PER_SEC / 1000));

  return counting_triangles_verifier_execute_next_round(verifier, values);
}

CountingTrianglesVerificationResult
counting_triangles_verifier_execute_next_round(
    CountingTrianglesVerifier *verifier, Vector *values) {
  clock_t start = clock();

  long eval_0 =
      evaluate_univariate_lagrange_interpolation(values, 0, verifier->base);
  long eval_1 =
      evaluate_univariate_lagrange_interpolation(values, 1, verifier->base);

  __int128_t tmp = (__int128_t)eval_0 + (__int128_t)eval_1;

  if (modulo(tmp, verifier->base) !=
      verifier->claimed_results->values[verifier->round]) {
    verifier->round++;
    CountingTrianglesVerificationResult verification_result = {.result = 0,
                                                               .finished = 1};
    return verification_result;
  }

  long chosen_value =
      (long)floor(((double)rand() / RAND_MAX) * (verifier->base - 1));
  verifier->chosen_values->values[verifier->round] = chosen_value;
  verifier->claimed_results->values[verifier->round + 1] =
      evaluate_univariate_lagrange_interpolation(values, chosen_value,
                                                 verifier->base);

  verifier->time_taken += ((clock() - start) / (CLOCKS_PER_SEC / 1000));

  if (verifier->round == verifier->last_round)
    return counting_triangles_verifier_execute_last_round(verifier);

  verifier->round++;

  CountingTrianglesVerificationResult verification_result = {
      .result = 1, .chosen_value = chosen_value};

  return verification_result;
}

CountingTrianglesVerificationResult
counting_triangles_verifier_execute_last_round(
    CountingTrianglesVerifier *verifier) {
  clock_t start = clock();

  verifier->round++;
  // TODO: free
  Vector *indexes_1 = create_vector(verifier->power);
  Vector *indexes_2 = create_vector(verifier->power);
  for (unsigned int i = 0; i < verifier->power; i++) {
    indexes_1->values[i] = verifier->chosen_values->values[i];
    indexes_2->values[i] = verifier->chosen_values->values[verifier->power + i];
  }

  // TODO: free
  MatMultVerifier *mat_mult_verifier = create_mat_mult_verifier(
      verifier->adjacency_matrix, verifier->adjacency_matrix, indexes_1,
      indexes_2, verifier->base);

  verifier->time_taken += ((clock() - start) / (CLOCKS_PER_SEC / 1000));

  MatMultProofStartResult mat_mult_proof_start_result =
      counting_triangles_prover_start_mat_mult_proof(verifier->prover,
                                                     indexes_1, indexes_2);

  start = clock();

  MatMultVerificationResult mat_mult_verification_result =
      mat_mult_verifier_start_validation(mat_mult_verifier,
                                         mat_mult_proof_start_result.result,
                                         mat_mult_proof_start_result.values);

  verifier->time_taken += ((clock() - start) / (CLOCKS_PER_SEC / 1000));

  printf("MatMult Prover Round 1: %ld, ", mat_mult_proof_start_result.result);
  print_vector(mat_mult_proof_start_result.values);
  printf("\n");
  printf("MatMult Verifier Round 1: %d, %ld \n",
         mat_mult_verification_result.result,
         mat_mult_verification_result.chosen_value);

  while (!mat_mult_verification_result.finished) {
    // TODO: free
    Vector *values = counting_triangles_prover_execute_next_round(
        verifier->prover, mat_mult_verification_result.chosen_value);

    start = clock();

    mat_mult_verification_result =
        mat_mult_verifier_execute_next_round(mat_mult_verifier, values);

    verifier->time_taken += ((clock() - start) / (CLOCKS_PER_SEC / 1000));

    printf("MatMult Prover Round %d: ",
           verifier->prover->round - verifier->power * 2);
    print_vector(values);
    printf("\n");

    printf("MatMult Verifier Round %d: %d, %ld \n", mat_mult_verifier->round,
           mat_mult_verification_result.result,
           mat_mult_verification_result.chosen_value);

    if (!mat_mult_verification_result.result) {
      CountingTrianglesVerificationResult verification_result = {.result = 0,
                                                                 .finished = 1};
      return verification_result;
    }
  }

  start = clock();

  Vector *adjacency_matrix_function =
      matrix_to_function(verifier->adjacency_matrix);

  __int128_t tmp =
      (__int128_t)mat_mult_proof_start_result.result *
      (__int128_t)evaluate_multilinear_extension(
          adjacency_matrix_function, verifier->chosen_values, verifier->base);

  long result = modulo(tmp, verifier->base);

  CountingTrianglesVerificationResult verification_result = {
      .result = result == verifier->claimed_results->values[verifier->round],
      .finished = 1,
      .triangles = verifier->claimed_results->values[0]};

  verifier->time_taken += ((clock() - start) / (CLOCKS_PER_SEC / 1000));

  return verification_result;
}