#!/bin/sh

echo "compiling compr.c..."
gcc src/compr.c src/huffman.c -o compr

if [ $? -eq 0 ]; then
    echo "build successful: ./compr"
else
    echo "build failed"
fi
