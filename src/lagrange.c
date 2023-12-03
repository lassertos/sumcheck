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
      mpf_clears(Xk, ik, NULL);
    }

    mpf_mul(inner_result, inner_result, vector_i);
    mpf_add(result, result, inner_result);

    mpf_clears(inner_result, inner_result_div, vector_i, NULL);
  }

  mpz_set_f(result_int, result);
  mpz_mod_ui(result_int, result_int, base);

  unsigned long final_result = mpz_get_ui(result_int);

  mpf_clear(result);
  mpz_clear(result_int);

  return final_result;
}

unsigned long evaluate_lagrange_basis_polynomial(unsigned long w, Vector *x,
                                                 long base) {
  __int128_t result = 1;

  for (unsigned int i = 0; i < x->size; i++) {
    __int128_t tmp = (w & (0x1 << (x->size - (i + 1)))) > 0
                         ? result * x->values[i]
                         : result * (1L - x->values[i]);
    result = modulo(tmp, base);
  }

  return result;
}

Vector **build_needed_tables(Vector *base_values, long base) {
  long size = lround(log2((double)base_values->size));
  Vector **needed_tables = malloc(sizeof(Vector) * size);
  needed_tables[size - 1] = copy_vector(base_values);

  for (int i = size - 2; i >= 0; i--) {
    needed_tables[i] = create_vector(needed_tables[i + 1]->size / 2);
    for (unsigned int j = 0; j < needed_tables[i]->size; j++) {
      needed_tables[i]->values[j] = needed_tables[i + 1]->values[j * 2] ||
                                    needed_tables[i + 1]->values[j * 2 + 1];
    }
  }

  return needed_tables;
}

unsigned long evaluate_multilinear_extension(Vector *base_values,
                                             Vector *vector, long base) {
  __int128_t tmp_result = 0;
  unsigned long result = 0;

  Vector *table = create_vector(1);
  table->values[0] = 1;

  for (unsigned int i = 0; i < vector->size; i++) {
    Vector *tmp_table = create_vector(table->size * 2);
    for (unsigned int j = 0; j < table->size; j++) {
      tmp_table->values[j * 2] = modulo((__int128_t)table->values[j] *
                                            (__int128_t)(1 - vector->values[i]),
                                        base);

      tmp_table->values[j * 2 + 1] = modulo(
          (__int128_t)table->values[j] * (__int128_t)vector->values[i], base);
    }
    destroy_vector(table);
    table = tmp_table;
  }

  for (unsigned long i = 0; i < base_values->size; i++) {
    if (!base_values->values[i]) {
      continue;
    }
    __int128_t v1 = base_values->values[i];
    __int128_t v2 = table->values[i];
    __int128_t tmp = v1 * v2;
    tmp_result += modulo(tmp, base);
    tmp_result = modulo(tmp_result, base);
  }

  destroy_vector(table);

  result = tmp_result;
  return result;
}

unsigned long evaluate_multilinear_extension_with_needed_tables(
    Vector *base_values, Vector *vector, long base, Vector **needed_tables) {
  __int128_t tmp_result = 0;
  unsigned long result = 0;

  Vector *table = create_vector(1);
  table->values[0] = 1;

  for (unsigned int i = 0; i < vector->size; i++) {
    Vector *tmp_table = create_vector(table->size * 2);
    for (unsigned int j = 0; j < table->size; j++) {
      if (needed_tables[i]->values[j * 2])
        tmp_table->values[j * 2] = modulo(
            (__int128_t)table->values[j] * (__int128_t)(1 - vector->values[i]),
            base);

      if (needed_tables[i]->values[j * 2 + 1])
        tmp_table->values[j * 2 + 1] = modulo(
            (__int128_t)table->values[j] * (__int128_t)vector->values[i], base);
    }
    destroy_vector(table);
    table = tmp_table;
  }

  for (unsigned long i = 0; i < base_values->size; i++) {
    if (!base_values->values[i]) {
      continue;
    }
    __int128_t v1 = base_values->values[i];
    __int128_t v2 = table->values[i];
    __int128_t tmp = v1 * v2;
    tmp_result += modulo(tmp, base);
    tmp_result = modulo(tmp_result, base);
  }

  destroy_vector(table);

  result = tmp_result;
  return result;
}