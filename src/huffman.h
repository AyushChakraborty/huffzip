#ifndef HUFFMAN_H
#define HUFFMAN_H

typedef enum { IS_NONLEAF, IS_CHAR } nodeType;

typedef struct node {
  int freq;
  union {
    int leaf;
    char c;
  } type;
  struct node *left;
  struct node *right;
  nodeType tag;
} NODE;

void heapify(NODE **nodes, int non_null_len, int start_index);
void build_min_heap(NODE **nodes, int non_null_len);
void extractMin(NODE **nodes, int *non_null_len, NODE **selected_nodes);
void insertNode(NODE **nodes, int *non_null_len, NODE **selected_nodes);
NODE *huff_tree(int *freqs, int len);

#endif
