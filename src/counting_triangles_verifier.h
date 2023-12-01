#ifndef COUNTING_TRIANGLES_VERIFIER_H
#define COUNTING_TRIANGLES_VERIFIER_H

#include "counting_triangles_prover.h"
#include "graph.h"
#include "lagrange.h"
#include "mat_mult_verifier.h"
#include "modulo.h"
#include "vector.h"

typedef struct {
  Vector *claimed_results;
  Vector *chosen_values;
  Matrix *adjacency_matrix;
  long base;
  unsigned int power;
  unsigned int last_round;
  unsigned int round;
  CountingTrianglesProver *prover;
  unsigned long time_taken;
} CountingTrianglesVerifier;

typedef struct {
  unsigned char result;
  unsigned char finished;
  long chosen_value;
  long triangles;
} CountingTrianglesVerificationResult;

extern CountingTrianglesVerifier *
create_counting_triangles_verifier(Graph *graph,
                                   CountingTrianglesProver *prover, long base);
extern CountingTrianglesVerificationResult
counting_triangles_verifier_start_validation(
    CountingTrianglesVerifier *verifier, long claimed_result, Vector *values);
extern CountingTrianglesVerificationResult
counting_triangles_verifier_execute_next_round(
    CountingTrianglesVerifier *verifier, Vector *values);
extern CountingTrianglesVerificationResult
counting_triangles_verifier_execute_last_round(
    CountingTrianglesVerifier *verifier);
extern void
destroy_counting_triangles_verifier(CountingTrianglesVerifier *verifier);

#endif