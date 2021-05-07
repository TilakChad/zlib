#ifndef PRIORITY_MINHEAP_H
#define PRIORITY_MINHEAP_H

#include <stdint.h>
#include "huffman_tree.h"
#include <stdio.h>
// Initializes priority minheap of given size
// Priority queue will only manage the pointer to nodes and won't allocate or
// deallocate on its own

typedef struct priority_minheap
{
    uint16_t size;
    uint16_t current_index;
    node** queue;
} priority_minheap;

priority_minheap init_minheap(uint16_t );

// Inserts node into the priority queue 
int8_t  insert_pqueue(priority_minheap* pqueue, node* );

// by default the top element will be top_down_heapified

void top_down_heapify(priority_minheap*);

// by default, the last inserted element will be bottom up heapified
void bottom_up_heapify(priority_minheap*);

// Min element will be removed form the heap 
node* remove_min(priority_minheap*);


#endif 
