#!/bin/bash

echo "Compiling huff.c"
gcc src/huff.c src/compr.c src/decompr.c src/huffman.c -o huff

if [ $? -eq 0 ]; then
    echo "Build successful: ./huff"
else
    echo "Build failed"
fi
