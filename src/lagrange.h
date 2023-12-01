#ifndef LAGRANGE_H
#define LAGRANGE_H

#include "vector.h"

extern unsigned long
evaluate_univariate_lagrange_interpolation(Vector *vector, long X, long base);
extern unsigned long evaluate_multilinear_extension(Vector *base_values,
                                                    Vector *vector, long base);
extern unsigned long evaluate_lagrange_basis_polynomial(unsigned long w,
                                                        Vector *x, long base);

#endif