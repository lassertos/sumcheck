#ifndef GRAPH_H
#define GRAPH_H

#include "matrix.h"

typedef struct {
  unsigned long node_a;
  unsigned long node_b;
} Edge;

typedef struct {
  unsigned long nodes;
  unsigned long size_edges;
  Edge *edges;
} Graph;

extern Graph *create_random_graph(unsigned long, unsigned int);
extern Matrix *get_adjacency_matrix(Graph *);
extern unsigned long count_triangles(Graph *);
extern void destroy_graph(Graph *graph);

#endif