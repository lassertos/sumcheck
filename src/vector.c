#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

Vector *create_vector(unsigned long size) {
  Vector *vector = malloc(sizeof(Vector));

  vector->size = size;
  vector->values = malloc(sizeof(long) * size);

  return vector;
}

Vector *create_vector_from(unsigned long size, long *values) {
  Vector *vector = create_vector(size);

  for (unsigned long i = 0; i < size; i++) {
    vector->values[i] = values[i];
  }

  return vector;
}

void print_vector(Vector *vector) {
  printf("[");
  for (unsigned int i = 0; i < vector->size; i++) {
    if (i == 0)
      printf("%ld", vector->values[i]);
    else
      printf(",%ld", vector->values[i]);
  }
  printf("]");
}

BinaryVector *create_binary_vector_from(unsigned long size, long value) {
  if (size > 64 || size <= 0)
    return NULL;

  BinaryVector *binary_vector = malloc(sizeof(BinaryVector));

  binary_vector->size = size;
  binary_vector->values = malloc(sizeof(long) * size);

  for (unsigned long i = size; i > 0; i--) {
    binary_vector->values[i - 1] = value & 1;
    value >>= 1;
  }

  return binary_vector;
}

void print_binary_vector(BinaryVector *binary_vector) {
  printf("[");
  for (unsigned int i = 0; i < binary_vector->size; i++) {
    if (i == 0)
      printf("%u", binary_vector->values[i]);
    else
      printf(",%u", binary_vector->values[i]);
  }
  printf("]");
}