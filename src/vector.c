#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

Vector *create_vector(unsigned long size) {
  Vector *vector = malloc(sizeof(Vector));

  vector->size = size;
  vector->values = malloc(sizeof(long) * size);

  for (unsigned long i = 0; i < size; i++) {
    vector->values[i] = 0;
  }

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

void destroy_vector(Vector *vector) {
  free(vector->values);
  free(vector);
}