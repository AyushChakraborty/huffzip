#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// scanning through the file to get the freqs of all the chars, hashed using
// just the ascii values, for now it only handles ascii, but will scale it later
// to handle unicode

int *char_freqs(FILE *file) {
  FILE *file_temp = file;
  int *freqs = (int *)calloc(256, sizeof(int));
  int c;

  while ((c = fgetc(file_temp)) != EOF) {
    // printf("%d\n", c);
    freqs[c] += 1;
  }
  // the file is represented by lots of bits, but here, only the ascii value is
  // read, which goes from 0 to 255
  return freqs;
}

typedef enum { IS_NONLEAF, IS_CHAR } nodeType;

typedef struct node {
  int freq;

  union type {
    int leaf;
    char c;
  } type;

  struct node *left;
  struct node *right;
  nodeType tag;

} NODE; // node for the huffman tree

void heapify(NODE **nodes, int non_null_len, int start_index) {
  /*
   * this function heapifies from the node at start_index to the nodes below
   *
   * params:
   *      node: array of pointer to nodes of the huffman tree
   *      non_null_len: the length after which NULL nodes exist
   *      start_index: the initial parent node from where heapify begins
   */

  int smallest = start_index;
  int left = 2 * start_index + 1;
  int right = 2 * start_index + 2;

  if (left < non_null_len && nodes[left]->freq < nodes[smallest]->freq) {
    smallest = left;
  }
  if (right < non_null_len && nodes[right]->freq < nodes[smallest]->freq) {
    smallest = right;
  }

  if (smallest != start_index) {
    NODE *temp = nodes[start_index];
    nodes[start_index] = nodes[smallest];
    nodes[smallest] = temp;

    // start recursive heapify again from the index which was involved in the
    // swap, to ensure that the global min heap property remains
    heapify(nodes, non_null_len, smallest);
  }
}

void build_min_heap(NODE **nodes, int non_null_len) {
  /* the loop instructs each node from last parent to the first one to fix its
   * own subtree by calling the heapify function
   *
   * params:
   *       nodes: array of pointers to nodes
   *       non_null_len: length in the array after which NULL nodes are
   * present
   */
  for (int i = (non_null_len - 2) / 2; i >= 0; i--) {
    heapify(nodes, non_null_len, i);
  }
}

void extractMin(NODE **nodes, int *non_null_len, NODE **selected_nodes) {
  /*
   * function to extract/remove the first two min nodes, and heapify the
   * remaining part of the heap array each time
   *
   * params:
   *    len: int, total size of the array
   *    nodes: pointer to array of pointer to NODE, the primary array
   *   non_null_len: pointer to length of actual heap
   *   selected_nodes: array of pointer to NODE, the selected nodes populate
   * this array
   */

  NODE *min_node_0 = nodes[0];
  NODE *min_node_1 = NULL;

  // since first node is removed, we insert the last leaf there, set the index
  // of the last leaf to be NULL
  nodes[0] = nodes[(*non_null_len) - 1];
  nodes[(*non_null_len) - 1] = NULL;
  (*non_null_len)--; // length of heap has decreased by one

  build_min_heap(nodes, *non_null_len);

  min_node_1 = nodes[0];

  nodes[0] = nodes[(*non_null_len) - 1];
  nodes[(*non_null_len) - 1] = NULL;
  (*non_null_len)--;

  build_min_heap(nodes, *non_null_len);

  selected_nodes[0] = min_node_0;
  selected_nodes[1] = min_node_1;
}

void insertNode(NODE **nodes, int *non_null_len, NODE **selected_nodes) {
  /* merge the selected nodes, where the nodes selected have already been
   * removed from the nodes array the new node that will be added, will be a
   * non leaf node which is signified by its value set to 0 (although the
   * exact value has no purpose, since the char cannot be set, and that was
   * the point with non leaf nodes)
   *
   * params:
   *      nodes: array of pointers to nodes of the huffman tree
   *      non_null_len: length within the nodes array after which NULL start
   *      selected_nodes: nodes with the most freqs that are selected for
   * merger
   */

  int freq_0 = selected_nodes[0]->freq;
  int freq_1 = selected_nodes[1]->freq;

  NODE *local_root = (NODE *)malloc(sizeof(NODE));
  local_root->type.leaf =
      0; // its not a leaf so the character it holds does not matter
  local_root->tag = IS_NONLEAF;
  local_root->freq = freq_0 + freq_1;

  if (freq_0 > freq_1) {
    local_root->left = selected_nodes[1];
    local_root->right = selected_nodes[0];
  } else {
    local_root->left = selected_nodes[0];
    local_root->right = selected_nodes[1];
  }

  // time to insert the local root in the heap tree
  nodes[*non_null_len] = local_root;
  (*non_null_len)++; // since a new node has been inserted

  build_min_heap(nodes, *non_null_len);
}

NODE *huff_tree(int *freqs, int len) {
  /* make nodes for all chars with non zero freq, these nodes will eventually
   * go on to become the leaf nodes within the huffman tree
   *
   * params:
   *      freqs: array of freqs for all 256 ascii chars, where some of them
   * are 0
   *
   *      len: in this case, 256, signifies all the ascii chars
   *
   * returns:
   *      pointer to the root node of the eventual huffman tree
   */
  NODE *nodes[len]; // all the nodes of the non-0 chars will be at the start
                    // of the array

  for (int i = 0; i < len; i++) {
    nodes[i] = NULL;
  }
  int count = 0;
  for (int i = 0; i < len; i++) {
    if (freqs[i] != 0) {
      NODE *new = (NODE *)malloc(sizeof(NODE));
      new->freq = freqs[i];
      new->type.c = (char)i;
      new->tag = IS_CHAR;
      new->left = NULL;
      new->right = NULL;
      nodes[count] = new;
      count++;
    }
  }

  // iterative process of making the huffman tree
  // loop goes on till only one node is left
  int start_index;
  NODE *selected_nodes[2];

  build_min_heap(&nodes[0], count);

  while (count != 1) {
    extractMin(&nodes[0], &count, selected_nodes);
    insertNode(&nodes[0], &count, selected_nodes);
  }

  return nodes[0]; // eventually the first element will be the root of the
                   // huffman tree
}

void print_tree(NODE *root, const char *prefix, int is_left) {
  /* this function uses preorder traversal of the tree to print the huffman
   * tree in a visual format
   */

  if (root == NULL)
    return;

  printf("%s", prefix);
  printf(is_left ? "├── " : "└── ");

  if (root->tag == IS_CHAR)
    printf("[%c] (freq: %d)\n", root->type.c, root->freq);
  else if (root->tag == IS_NONLEAF)
    printf("* (freq: %d)\n", root->freq);

  char new_prefix[256];
  snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_left ? "│   " : "    ");

  print_tree(root->left, new_prefix, 1);
  print_tree(root->right, new_prefix, 0);
}

// TODO: function to traverse through the huffman tree and make the encoding
// array

void make_encoding_array(NODE *root, int len, char *buffer, int buf_len, char *codes[len], int depth) {

  if (root == NULL) {
    // unwrite the 0 or 1 written, since no node exists here
    //printf("just entered NULL node\n");
    if (strlen(buffer) > 0) {
      //printf("able to enter NULL node\n");
      buffer[strlen(buffer) - 1] = '\0';
    }
    depth--;
    return;
  }

  if (root->left == NULL && root->right == NULL) {
    //printf("able to enter the leaf\n");
    printf("assigning code for %c: %s\n", root->type.c, buffer);
    strcpy(codes[(int)root->type.c], buffer);
    printf("depth of leaf: %d\n\n", depth);
    depth--;
    return;
  }

  depth++;

  buffer[strlen(buffer)] = '0';
  buffer[strlen(buffer)+1] = '\0';
  printf("string while going in the left halves: %s\n", buffer);
  //printf("able to get past the left nodes\n");
  make_encoding_array(root->left, len, buffer, buf_len, codes, depth);
  printf("depth before transitioning: %d\n", depth);
  buffer[depth-1] = '1';
  buffer[depth] = '\0';

  for (int i=depth+1; i<buf_len; i++) {
        buffer[i] = '\0';     //setting the rest of the bits to be null to, to avoid issues later 
    }

  printf("string length right before transitioning: %lu\n", strlen(buffer));
  printf("string right before transitioning: %s\n", buffer);
  //printf("able to get to the right nodes\n");
  make_encoding_array(root->right, len, buffer, buf_len, codes, depth);
}

void write_to_file(NODE *root, FILE *new_file, FILE *org_file) {
  // take the codes array, and replace the chars by these bit codes
  // new_file must be write mode, org_file in read mode
}

int main() {
  FILE *file = fopen("texts/s3.txt", "r");
  int *freqs = char_freqs(file);
  for (int i = 0; i < 256; i++) {
    printf("%d\n", freqs[i]);
  }

  NODE *root = huff_tree(freqs, 256);
  print_tree(root, "", 0);

  int len = 256;
  // stores the huffman codes for each of the possible ascii chars
  char *codes[len];
  int max_code_len = 256;

  for (int i = 0; i < len; i++) {
    codes[i] = (char *)malloc(max_code_len);
  }

  char *buffer = (char *)malloc(max_code_len * sizeof(char));
  printf("entring here!\n");
  make_encoding_array(root, len, buffer,max_code_len, codes, 0);

    //printing the codes
    for (int i=0; i< len; i++) {
        if (freqs[i] != 0) {
            printf("%c: %s\n", i, codes[i]);
        }
    }

  // cleanup
  for (int i = 0; i < len; i++) {
    free(codes[i]);
  }
  free(buffer);

  fclose(file);
  return 0;
}
