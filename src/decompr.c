#include "huff.h"
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
        fputc(temp->type.c, dec_file);
        temp = root; // reset pointer back to root of tree
      }
    }
    num_bits -= 8;
  }
}

void decompress(FILE *enc_file, FILE *dec_file) {

  // read the encoded file to get the freqs, make the huffman tree from it
  if (enc_file == NULL) {
    printf("could not open the file\n");
    return;
  }

  // reconstruct the huffman tree and the num of actual bits
  // so first get the freq table from the text file
  int *freqs = (int *)calloc(256, sizeof(int));
  char line[32];
  int num = 1;
  int num_bits = 0;

  // read the num of bits actually written
  fread(&num_bits, sizeof(int), 1, enc_file);
  fgetc(enc_file); // read the newline

  char ch;
  int freq;

  while (fgets(line, sizeof(line), enc_file)) {
    size_t len = strlen(line);

    if (strcmp(line, "\n") == 0) {
      if (num != 2) {
        num++;
      } else {
        break;
      }
      // peep at the next line to see if a freq exists, if yes its the case of
      // \n
      if (fgets(line, sizeof(line), enc_file)) {
        if (sscanf(line, "%d", &freq) == 1) {
          freqs[(int)'\n'] = freq;
        }
      }
      continue;
    }
    if (sscanf(line, "%c %d", &ch, &freq) == 2) {
      freqs[(int)ch] = freq;
    }
  }

  // for (int i = 0; i < 256; i++) {
  //  if (freqs[i] != 0) {
  //   printf("%c %d\n", i, freqs[i]);
  //}
  // }

  NODE *root = huff_tree(freqs, 256);
  // now the fp is at the location where the actual content is, so can write
  // the decoded contents to dec_file

  if (dec_file == NULL) {
    printf("could not open the file\n");
    return;
  }

  traverse_huffman_tree(root, enc_file, dec_file, num_bits);
}
