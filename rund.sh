#!/bin/sh

echo "compiling decompr.c..."
gcc src/decompr.c src/huffman.c -o decompr

if [ $? -eq 0 ]; then
    echo "build successful: ./decompr"
else
    echo "build failed"
fi

