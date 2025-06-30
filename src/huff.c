#include "huff.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

long file_size(const char *file) {
  struct stat st;
  if (stat(file, &st) == 0) {
    return st.st_size;
  }
  return -1;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: huff <compress|decompress> <input> <output>\n");
    return 1;
  }

  if (strcmp(argv[1], "compress") == 0) {
    FILE *in = fopen(argv[2], "r");
    FILE *out = fopen(argv[3], "w");
    // your compress function
    compress(in, out);
    printf("Huffman says: Compressed!\n");

    long input_size = file_size(argv[2]);
    long output_size = file_size(argv[3]);

    if (input_size >= 0 && output_size >= 0) {
      printf("Input size: %ld bytes\n", input_size);
      printf("Output size: %ld bytes\n", output_size);
      if (strcmp(argv[1], "compress") == 0) {
        double ratio = 100.0 * (1.0 - (double)output_size / input_size);
        printf("Compression ratio: %.2f%%\n", ratio);
      }
    }

    fclose(in);
    fclose(out);
  } else if (strcmp(argv[1], "decompress") == 0) {
    FILE *in = fopen(argv[2], "r");
    FILE *out = fopen(argv[3], "w");
    // your decompress function
    decompress(in, out);
    printf("Huffman says: Decompressed!\n");
    fclose(in);
    fclose(out);
  } else {
    fprintf(stderr, "Unknown mode: %s\n", argv[1]);
    return 1;
  }

  return 0;
}
