#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H
#include <stdint.h>

typedef struct node {
    int16_t value;
    int16_t frequency;
    struct node* left;
    struct node* right;
} node;

int huffman_coding(int32_t*, unsigned, int32_t*, unsigned, int max_code_length_allowed);
void code_length(node*, int32_t , int32_t*, unsigned);

void print_tree(node*, char* , int32_t );

// Take the first node of the huffman tree and deallocates 
void cleanup_tree(node*);

// Just placed it here for reference purpose of failed attempt 
int rebalance_huffman_tree(node*);

// Rebalancing huffman tree if required
int rebalance_huffman_tree_2D(node**);

// Determines the height of the huffman tree. It returns actual height + 1
int height(node*);

#endif
