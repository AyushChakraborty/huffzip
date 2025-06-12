#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

  // read the encoded file to get the freqs, make the huffman tree from it
  FILE *enc_file = fopen("texts/output.txt", "r");
  FILE *dec_file =
      fopen("texts/new.txt",
            "w"); // file to which the decoded contents will be written

  // reconstruct the huffman tree
  // so first get the freq table from the text file
  int *freqs = (int *)calloc(256, sizeof(char));
  char line[32];
  int num = 1;
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

  return 0;
}
