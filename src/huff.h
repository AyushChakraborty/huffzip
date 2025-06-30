#ifndef HUFF_H
#define HUFF_H

#include <stdio.h>

void compress(FILE *in_file, FILE *out_file);
void decompress(FILE *enc_file, FILE *dec_file);

#endif
