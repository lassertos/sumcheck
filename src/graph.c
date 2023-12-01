#include "graph.h"
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>

Graph *create_random_graph(unsigned long nodes) {
  Graph *graph = malloc(sizeof(Graph));

  graph->nodes = nodes;

  graph->edges = malloc(sizeof(Edge) * (((nodes * nodes)) / 2) - nodes);

  for (unsigned int i = 0; i < nodes; i++) {
    for (unsigned int j = i + 1; j < nodes; j++) {
      if (rand() % 100 < 20) {
        graph->edges[graph->size_edges].node_a = i;
        graph->edges[graph->size_edges].node_b = j;
        graph->size_edges++;
      }
    }
  }

  return graph;
}

Matrix *get_adjacency_matrix(Graph *graph) {
  Matrix *matrix = create_matrix(graph->nodes, graph->nodes);

  for (unsigned int i = 0; i < graph->size_edges; i++) {
    matrix->values[graph->edges[i].node_a][graph->edges[i].node_b] = 1;
    matrix->values[graph->edges[i].node_b][graph->edges[i].node_a] = 1;
  }

  return matrix;
}

unsigned long count_triangles(Graph *graph) {
  Matrix *adjacency_matrix = get_adjacency_matrix(graph);

  unsigned long result = 0;
  for (unsigned int i = 0; i < graph->nodes; i++) {
    for (unsigned int j = i + 1; j < graph->nodes; j++) {
      for (unsigned int k = j + 1; k < graph->nodes; k++) {
        if (adjacency_matrix->values[i][j] && adjacency_matrix->values[j][k] &&
            adjacency_matrix->values[i][k])
          result++;
      }
    }
  }

  free(adjacency_matrix);

  return result;
}

// unsigned long count_triangles(Graph *graph)
// {
//     Matrix *adjacency_matrix = get_adjacency_matrix(graph);

//     unsigned long result = 0;
//     for (unsigned int i = 0; i < graph->nodes; i++)
//     {
//         for (unsigned int j = 0; j < graph->nodes; j++)
//         {
//             for (unsigned int k = 0; k < graph->nodes; k++)
//             {
//                 if (adjacency_matrix->values[i][j] &&
//                 adjacency_matrix->values[j][k] &&
//                 adjacency_matrix->values[i][k])
//                     result++;
//             }
//         }
//     }

//     free(adjacency_matrix);

//     return result / 6;
// }