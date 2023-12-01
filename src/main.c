#include "counting_triangles_prover.h"
#include "counting_triangles_verifier.h"
#include "graph.h"
#include "lagrange.h"
#include "matrix.h"
#include "modulo.h"
#include "vector.h"
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
  srand(1);

  unsigned long size = 1000;
  long max = (1L << 62) - 1;

  mpz_t prime;
  mpz_init_set_ui(prime, 1);
  mpz_mul_ui(prime, prime, 6);
  mpz_mul_ui(prime, prime, size);
  mpz_mul_ui(prime, prime, size);
  mpz_mul_ui(prime, prime, size);
  mpz_add_ui(prime, prime, 1);
  mpz_nextprime(prime, prime);
  if (mpz_cmp_si(prime, max) > 0)
    return 1;

  long base = mpz_get_si(prime);

  printf("Base: %ld\n", base);

  clock_t start = clock();
  Graph *graph = create_random_graph(size);
  clock_t stop = clock();
  printf("Graph Creation Time Taken: %ldms\n",
         (stop - start) / (CLOCKS_PER_SEC / 1000));

  CountingTrianglesProver *prover =
      create_counting_triangles_prover(graph, base);
  CountingTrianglesVerifier *verifier =
      create_counting_triangles_verifier(graph, prover, base);

  CountingTrianglesProofStartResult proof_start_result =
      counting_triangles_prover_start(prover);
  CountingTrianglesVerificationResult verification_result =
      counting_triangles_verifier_start_validation(
          verifier, proof_start_result.result, proof_start_result.values);

  printf("Prover Round 1: %ld, ", proof_start_result.result);
  print_vector(proof_start_result.values);
  printf("\n");
  printf("Verifier Round 1: %d, %ld, %ld \n", verification_result.result,
         verification_result.chosen_value,
         verifier->claimed_results->values[verifier->round]);

  while (!verification_result.finished) {
    // TODO: free values
    Vector *values = counting_triangles_prover_execute_next_round(
        prover, verification_result.chosen_value);

    printf("Prover Round %d: ", prover->round);
    print_vector(values);
    printf("\n");

    verification_result =
        counting_triangles_verifier_execute_next_round(verifier, values);

    printf("Verifier Round %d: %d, %ld, %ld \n", verifier->round,
           verification_result.result, verification_result.chosen_value,
           verifier->claimed_results->values[verifier->round]);

    if (!verification_result.result)
      break;
  }

  printf("Proving Time Taken: %ldms\n", prover->time_taken);
  printf("Verification Time Taken: %ldms\n", verifier->time_taken);

  start = clock();
  unsigned long triangles = count_triangles(graph);
  stop = clock();
  printf("Counting Triangles Time Taken: %ldms\n",
         (stop - start) / (CLOCKS_PER_SEC / 1000));

  printf("Triangles: %lu\n", triangles);

  return 0;
}
