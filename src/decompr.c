#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void traverse_huffman_tree(NODE *root, FILE *enc_file, FILE *dec_file,
                           int num_bits) {
  /*
   * file here points to the start of the actual encoded text
   * */
  char byte;
  int bit;
  NODE *temp = root;
  while (num_bits > 0) {
    int bits_to_be_read = 8;
    if (num_bits < 8 && num_bits != 0) {
      bits_to_be_read = num_bits;
    }
    byte = fgetc(enc_file);

    // read bits
    for (int i = 0; i < bits_to_be_read; i++) {
      bit = (byte >> (7 - i)) & 1;

      // traverse the tree accordingly
      if (bit == 0) {
        temp = temp->left;
      } else if (bit == 1) {
        temp = temp->right;
      }

      if (temp->tag == IS_CHAR) {
        printf("%c", temp->type.c);
        temp = root; // reset pointer back to root of tree
      }
    }

    num_bits -= 8;
  }
}

int main() {

  // read the encoded file to get the freqs, make the huffman tree from it
  FILE *enc_file = fopen("texts/output.txt", "r");
  FILE *dec_file =
      fopen("texts/new.txt",
            "w"); // file to which the decoded contents will be written

  // reconstruct the huffman tree and the num of actual bits
  // so first get the freq table from the text file
  int *freqs = (int *)calloc(256, sizeof(char));
  char line[32];
  int num = 1;
  int counter = 0;
  int num_bits = 0;

  fread(&num_bits, sizeof(int), 1, enc_file);
  fgetc(enc_file); // read the newline

  while (fgets(line, sizeof(line), enc_file)) {

    if (strcmp(line, "\n") == 0) {
      if (num != 2) {
        num++;
      } else {
        break;
      }
    } else {
      if (line[1] != ' ') {
        freqs[(int)'\n'] = line[1] - '0';
      } else {
        freqs[(int)line[0]] = line[2] - '0';
      }
    }
  }
  NODE *root = huff_tree(freqs, 256);
  // all cool now
  // printf("num of bits actually present: %d\n", num_bits);

  // now the fp is at the location where the actual content is, so can write the
  // decoded contents to dec_file
  traverse_huffman_tree(root, enc_file, dec_file, num_bits);

  return 0;
}
