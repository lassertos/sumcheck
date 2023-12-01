#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
  unsigned int size;
  long *values;
} Vector;

typedef struct {
  unsigned int size;
  unsigned char *values;
} BinaryVector;

extern Vector *create_vector(unsigned long);
extern Vector *create_vector_from(unsigned long, long *);
extern void print_vector(Vector *vector);
extern void destroy_vector(Vector *vector);

#endif