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

extern BinaryVector *create_binary_vector_from(unsigned long, long);
extern void print_binary_vector(BinaryVector *binary_vector);

#endif