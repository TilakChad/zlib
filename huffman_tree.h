#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H
#include <stdint.h>

typedef struct node {
    int16_t value;
    int16_t frequency;
    struct node* left;
    struct node* right;
} node;

int huffman_coding(int32_t*, unsigned, int32_t*, unsigned);
void code_length(node*, int32_t , int32_t*, unsigned);

void print_tree(node*, char* , int32_t );

// Take the first node of the huffman tree and deallocates 
void cleanup_tree(node*); 
#endif
