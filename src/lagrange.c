#include "modulo.h"
#include "vector.h"
#include <gmp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

unsigned long evaluate_univariate_lagrange_interpolation(Vector *vector, long X,
                                                         long base) {
  mpf_t result;
  mpz_t result_int;
  mpf_init_set_si(result, 0);
  mpz_init(result_int);

  for (int i = 0; i < vector->size; i++) {
    mpf_t inner_result, inner_result_div, vector_i;
    mpf_init_set_si(inner_result, 1);
    mpf_init(inner_result_div);
    mpf_init_set_ui(vector_i, vector->values[i]);

    for (int k = 0; k < vector->size; k++) {
      if (k == i)
        continue;
      mpf_t Xk, ik;
      mpf_init_set_si(Xk, X - k);
      mpf_init_set_si(ik, i - k);

      mpf_div(inner_result_div, Xk, ik);
      mpf_mul(inner_result, inner_result, inner_result_div);
    }

    mpf_mul(inner_result, inner_result, vector_i);
    mpf_add(result, result, inner_result);
  }

  mpz_set_f(result_int, result);
  mpz_mod_ui(result_int, result_int, base);

  return mpz_get_ui(result_int);
}

unsigned long evaluate_lagrange_basis_polynomial(BinaryVector *w, Vector *x,
                                                 long base) {
  long result = 1;

  for (unsigned int i = 0; i < w->size; i++) {
    __int128_t tmp = w->values[i] == 1
                         ? (__int128_t)result * (__int128_t)x->values[i]
                         : (__int128_t)result * (__int128_t)(1L - x->values[i]);
    result = modulo(tmp, base);
  }

  return result;
}

unsigned long evaluate_multilinear_extension(Vector *base_values,
                                             Vector *vector, long base) {
  __int128_t tmp_result = 0;
  unsigned long result = 0;

  for (unsigned long i = 0; i < base_values->size; i++) {
    if (!base_values->values[i])
      continue;
    BinaryVector *binary_i = create_binary_vector_from(vector->size, i);
    __int128_t v1 = base_values->values[i];
    __int128_t v2 = evaluate_lagrange_basis_polynomial(binary_i, vector, base);
    __int128_t tmp = v1 * v2;
    tmp_result += modulo(tmp, base);
    tmp_result = modulo(tmp_result, base);
  }

  result = tmp_result;
  return result;
}
