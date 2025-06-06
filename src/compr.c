#include "huffman.h"
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

void print_tree(NODE *root, const char *prefix, int is_left) {
  /* this function uses preorder traversal of the tree to print the huffman
   * tree in a visual format
   *
   *params:
        root: root of the huffman tree
        prefix: the pattern to print, based on weather its the left or right
   branch is_left: flag to check for the left branch
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
  snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix,
           is_left ? "│   " : "    ");

  print_tree(root->left, new_prefix, 1);
  print_tree(root->right, new_prefix, 0);
}

void make_encoding_array(NODE *root, int len, char *buffer, int buf_len,
                         char *codes[len], int depth) {
  /* this function makes the encoding array from the huffman tree, by following
   in inorder traversal method
   * the algorithm is that any left branch taken will append 0 to the huffman
   code for a character, and a right
   * branch taken will append 1. This is tracked by the buffer, which is shared
   across the call stack frames
   *
   *params:
      root: the root of the huffman tree
      len: length of the codes array(which is 256 as standard)
      buffer: holds the huffman code for a leaf, as we go down the tree
      buf_len: length of the buffer
      codes: holds the codes for each of the ascii chars
      depth: current depth of the tree node is contained here
   * */

  if (root == NULL) {
    // unwrite the 0 or 1 written, since no node exists here
    // printf("just entered NULL node\n");
    if (strlen(buffer) > 0) {
      // printf("able to enter NULL node\n");
      buffer[strlen(buffer) - 1] = '\0';
    }
    depth--;
    return;
  }

  if (root->left == NULL && root->right == NULL) {
    // printf("able to enter the leaf\n");
    // printf("assigning code for %c: %s\n", root->type.c, buffer);
    strcpy(codes[(int)root->type.c], buffer);
    // printf("depth of leaf: %d\n\n", depth);
    depth--;
    return;
  }

  depth++;

  buffer[strlen(buffer)] = '0';
  buffer[strlen(buffer) + 1] = '\0';
  //  printf("string while going in the left halves: %s\n", buffer);
  // printf("able to get past the left nodes\n");
  make_encoding_array(root->left, len, buffer, buf_len, codes, depth);
  //  printf("depth before transitioning: %d\n", depth);
  buffer[depth - 1] = '1';
  buffer[depth] = '\0';

  for (int i = depth + 1; i < buf_len; i++) {
    buffer[i] = '\0'; // setting the rest of the bits to be null to, to avoid
                      // issues later
  }

  // printf("string length right before transitioning: %lu\n", strlen(buffer));
  // printf("string right before transitioning: %s\n", buffer);
  // printf("able to get to the right nodes\n");
  make_encoding_array(root->right, len, buffer, buf_len, codes, depth);
}

void write_to_file(NODE *root, FILE *new_file, FILE *org_file, int len,
                   char *codes[len], int *freq_table) {
  /*take the codes array, and replace the chars by these bit codes
  *new_file must be append mode, org_file in read mode
  *
  *params:
        root: root of the huffman tree
        new_file: write only pointer to a file
        org_file: read only pointer to a file, the original content resides here
  * */
  int c;
  int bit_pos = 0;
  unsigned char bit_buffer = 0;

  // for the headers, write the freq table as is, so that the huffman tree can
  // be reconstructed in the decoder side offloading the load from the encoder
  // side

  for (int i = 0; i < len; i++) {
    if (freq_table[i] != 0) {
      fprintf(new_file, "%c %d\n", i, freq_table[i]);
    }
  }
  fputc('\n', new_file); // so body starts after \n\n

  while ((c = fgetc(org_file)) != EOF) {
    char *code_str = codes[c];

    // change code_str to pure byte representation
    // the idea here is that each char is used as a 8 bit buffer

    for (int i = 0; i < strlen(code_str); i++) {
      if (bit_pos == 8) {
        // if bit_buffer is full
        printf("written this part\n");
        fputc(bit_buffer, new_file);
        // emptying buffer
        bit_buffer = 0;
        bit_pos = 0; // reset
      }

      if (code_str[i] == '1') {
        bit_buffer |= (1 << (7 - bit_pos));
      }
      bit_pos++;
    }
  }

  // empty any remaining bits in the buffer
  // have to also write bit_pos to track the garbage bits
  if (bit_pos > 0) {
    fputc(bit_buffer, new_file);
    // constants always to be written, always taking 2 more bytes
    fputc('\n', new_file);
    fputc(bit_pos, new_file);
  } else {
    // again constants, always taking 2 more bytes
    fputc('\n', new_file);
    fputc(0, new_file); // the bit_pos now is 0 byte, since all buffers are
                        // perfectly filled
  }
}

int main() {
  FILE *file = fopen("texts/s3.txt", "r");

  if (file == NULL) {
    printf("could not open the file\n");
    return 1;
  }

  int *freqs = char_freqs(file);

  // rewinding the file pointer, so that it can be used by write_to_file() again
  rewind(file);

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
  make_encoding_array(root, len, buffer, max_code_len, codes, 0);

  // printing the codes
  for (int i = 0; i < len; i++) {
    if (freqs[i] != 0) {
      printf("%c: %s\n", i, codes[i]);
    }
  }

  FILE *new_file = fopen("texts/output.txt", "a");
  if (new_file == NULL) {
    printf("could not open the file\n");
    return 1;
  }

  write_to_file(root, new_file, file, len, codes, freqs);

  // cleanup
  for (int i = 0; i < len; i++) {
    free(codes[i]);
  }
  free(buffer);

  fclose(file);
  fclose(new_file);

  return 0;
}
