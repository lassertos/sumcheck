#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

Vector *create_vector(unsigned long size) {
  Vector *vector = malloc(sizeof(Vector));

  vector->size = size;
  vector->values = calloc(sizeof(long) * size, sizeof(long));

  return vector;
}

Vector *copy_vector(Vector *vector) {
  Vector *copy = create_vector(vector->size);

  for (unsigned long i = 0; i < vector->size; i++) {
    copy->values[i] = vector->values[i];
  }

  return copy;
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