#!/bin/sh

echo "compiling compr.c..."
gcc compr.c huffman.c -o compr

if [ $? -eq 0 ]; then
    echo "build successful: ./compr"
else
    echo "build failed"
fi
