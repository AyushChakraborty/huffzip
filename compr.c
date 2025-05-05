#include <stdio.h>
#include <stdlib.h>

//scanning through the file to get the freqs of all the chars, hashed using just the ascii values
//for now it only handles ascii, but will scale it later to handle unicode
int *char_freqs(FILE *file) {
    FILE *file_temp = file;
    int *freqs = (int*)calloc(256, sizeof(int));
    int c;

    while ((c=fgetc(file_temp)) != EOF) {
        //printf("%d\n", c);
        freqs[c] += 1;
    }
    //the file is represented by lots of bits, but here, only the ascii value is read, which goes
    //from 0 to 255 
    return freqs;
}


typedef struct node {
    int freq;
    char c;
    struct node *left;
    struct node *right;
}NODE;  //node for the huffman tree

NODE *huff_tree(int *freqs, int len) {
    //make nodes for all chars with non zero freq
    NODE *nodes[len];     //all the nodes of the non-0 chars will be at the start of the array

    for (int i=0; i<len; i++) {
        nodes[i] = NULL;          //NULL init
    }
    int count = 0;
    for (int i=0; i<len; i++) {
        if (freqs[i] != 0) {
            NODE *new = (NODE*)malloc(sizeof(NODE));
            new->freq = freqs[i];
            new->c = (char)i;
            new->left = NULL;
            new->right = NULL;
            nodes[count] = new;
            count++;
        }
    }

    return nodes[0];       //eventually the first element will be the root of the huffman tree
}


int main() {
    FILE *file = fopen("texts/s1.txt", "r");
    int *freqs = char_freqs(file);
    for (int i=0; i<255; i++) {
        printf("%d\n", freqs[i]);
    }
    return 0;
}