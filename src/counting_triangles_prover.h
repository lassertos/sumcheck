#ifndef COUNTING_TRIANGLES_PROVER_H
#define COUNTING_TRIANGLES_PROVER_H

#include "graph.h"
#include "lagrange.h"
#include "mat_mult_prover.h"
#include "matrix.h"
#include "modulo.h"

typedef struct {
  Vector *chosen_values;
  Vector *values_a;
  Vector *values_b;
  Matrix *adjacency_matrix;
  Matrix *squared_adjacency_matrix;
  long base;
  unsigned int power;
  unsigned int last_round;
  unsigned int round;
  unsigned char mode;
  MatMultProver *mat_mult_prover;
  unsigned long time_taken;
} CountingTrianglesProver;

typedef struct {
  long result;
  Vector *values;
} CountingTrianglesProofStartResult;

extern CountingTrianglesProver *create_counting_triangles_prover(Graph *graph,
                                                                 long base);
extern CountingTrianglesProofStartResult
counting_triangles_prover_start(CountingTrianglesProver *prover);
extern Vector *
counting_triangles_prover_execute_next_round(CountingTrianglesProver *prover,
                                             long chosen_value);
extern MatMultProofStartResult counting_triangles_prover_start_mat_mult_proof(
    CountingTrianglesProver *prover, Vector *indexes_1, Vector *indexes_2);

#endif