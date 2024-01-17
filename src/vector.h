#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
  unsigned long size;
  long *values;
} Vector;

extern Vector *create_vector(unsigned long);
extern Vector *copy_vector(Vector *);
extern void print_vector(Vector *vector);
extern void destroy_vector(Vector *vector);

#endif