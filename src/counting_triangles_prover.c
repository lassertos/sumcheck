#include "counting_triangles_prover.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

CountingTrianglesProver *create_counting_triangles_prover(Graph *graph,
                                                          long base) {
  clock_t start = clock();
  CountingTrianglesProver *prover = malloc(sizeof(CountingTrianglesProver));

  prover->base = base;

  prover->adjacency_matrix = get_adjacency_matrix(graph);
  prover->power = (unsigned int)(ceil(log2(prover->adjacency_matrix->rows)));
  prover->squared_adjacency_matrix =
      square_adjacency_matrix(prover->adjacency_matrix);

  prover->mode = 0;
  prover->round = 0;
  prover->last_round = (prover->power * 3) - 1;

  prover->chosen_values = create_vector((prover->power * 3) - 1);

  prover->time_taken = ((clock() - start) / (CLOCKS_PER_SEC / 1000));

  return prover;
}

CountingTrianglesProofStartResult
counting_triangles_prover_start(CountingTrianglesProver *prover) {
  clock_t start = clock();
  prover->mode = 0;
  prover->round = 0;
  unsigned long size = 1UL << (prover->power * 2);
  unsigned long base_size = 1UL << prover->power;

  prover->values_a = create_vector(size);
  prover->values_b = create_vector(size);

  long result = 0;

  for (unsigned int i = 0; i < prover->adjacency_matrix->rows; i++) {
    for (unsigned int j = 0; j < prover->adjacency_matrix->columns; j++) {
      if (prover->adjacency_matrix->values[i][j])
        result += prover->squared_adjacency_matrix->values[i][j] *
                  prover->adjacency_matrix->values[i][j];
    }
  }

  for (unsigned int i = 0; i < prover->adjacency_matrix->rows; i++) {
    for (unsigned int j = 0; j < prover->adjacency_matrix->columns; j++) {
      prover->values_a->values[i * base_size + j] +=
          prover->squared_adjacency_matrix->values[i][j];
      prover->values_b->values[i * base_size + j] +=
          prover->adjacency_matrix->values[i][j];
    }
  }

  CountingTrianglesProofStartResult start_result = {
      .result = result,
      .values = counting_triangles_prover_execute_next_round(prover, -1)};

  prover->time_taken += ((clock() - start) / (CLOCKS_PER_SEC / 1000));

  return start_result;
}

Vector *
counting_triangles_prover_execute_next_round(CountingTrianglesProver *prover,
                                             long chosen_value) {
  clock_t start = clock();
  prover->round++;
  if (prover->mode == 1) {
    Vector *values = mat_mult_prover_execute_next_round(prover->mat_mult_prover,
                                                        chosen_value);
    prover->time_taken += ((clock() - start) / (CLOCKS_PER_SEC / 1000));
    return values;
  }

  Vector *new_values = create_vector(3);

  if (chosen_value >= 0) {
    prover->chosen_values->values[prover->round - 2] = chosen_value;
    unsigned int remaining_spaces = prover->power * 2 - (prover->round - 1);
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

    destroy_vector(prover->values_a);
    destroy_vector(prover->values_b);

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
    unsigned int remaining_spaces = prover->power * 2 - (prover->round - 1);
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

  prover->time_taken += ((clock() - start) / (CLOCKS_PER_SEC / 1000));

  return new_values;
}

MatMultProofStartResult counting_triangles_prover_start_mat_mult_proof(
    CountingTrianglesProver *prover, Vector *indexes_1, Vector *indexes_2) {
  clock_t start = clock();
  prover->mode = 1;
  prover->mat_mult_prover =
      create_mat_mult_prover(prover->adjacency_matrix, prover->adjacency_matrix,
                             indexes_1, indexes_2, prover->base);

  MatMultProofStartResult result = mat_mult_prover_start(
      prover->mat_mult_prover, prover->squared_adjacency_matrix);

  prover->time_taken += ((clock() - start) / (CLOCKS_PER_SEC / 1000));

  return result;
}

void destroy_counting_triangles_prover(CountingTrianglesProver *prover) {
  destroy_matrix(prover->adjacency_matrix);
  destroy_matrix(prover->squared_adjacency_matrix);
  destroy_vector(prover->chosen_values);
  destroy_vector(prover->values_a);
  destroy_vector(prover->values_b);
  destroy_mat_mult_prover(prover->mat_mult_prover);
  free(prover);
}