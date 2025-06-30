all:
	$(CC) src/huff.c src/compr.c src/decompr.c src/huffman.c -o huff
