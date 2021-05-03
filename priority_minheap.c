#include "priority_minheap.h"
#include <stdlib.h>

priority_minheap init_minheap(uint16_t size)
{
    priority_minheap pqueue;
    pqueue.size = size;
    pqueue.current_index = 0;
    pqueue.queue = malloc(sizeof(node*) * size);
    for (int i = 0; i < size; ++i)
	pqueue.queue[i] = NULL;
    return pqueue;
}
    
int8_t insert_pqueue(priority_minheap* pqueue, node* tree_node)
{
    // priority queue doesn't do any allocation or deallocation on tree nodes
    if (pqueue->current_index >= pqueue->size)
	return -1;
    pqueue->queue[pqueue->current_index++] = tree_node;

    bottom_up_heapify(pqueue);
    return 0;
}

void bottom_up_heapify(priority_minheap* pqueue)
{
    // bottom up heapify .. Take the last element of the heap and put it into correct position
    // first, lets some repositioning of the queue pointer

    node** queue = pqueue->queue - 1;
    // get the index of the last element 
    int k = pqueue->current_index;

    while(k>1 && (queue[k/2]->frequency > queue[k]->frequency))

    {
	// just exchange the pointer
	node* temp = queue[k/2];
	queue[k/2] = queue[k];
	queue[k] = temp;
	k = k / 2;
    }
    
}

node* remove_min(priority_minheap* pqueue)
{
    if (pqueue->current_index==0)
	return NULL;

    // Exchange the first and the last entry of the priority minheap

    node* to_return = pqueue->queue[0];
    pqueue->queue[0] = pqueue->queue[--pqueue->current_index];

    top_down_heapify(pqueue);
    return to_return;
}

void top_down_heapify(priority_minheap* pqueue)
{
    node** queue = pqueue->queue - 1;
    int k = 1;
    while(2*k<=pqueue->current_index)
    {
	int current = 2*k; 
	if (2*k != pqueue->current_index)
	{
	    if (queue[2*k]->frequency > queue[2*k+1]->frequency)
		current = 2*k+1;
	}

	if (queue[k]->frequency <= queue[current]->frequency)
	    break;
	else
	{
	    node* temp = queue[current];
	    queue[current] = queue[k];
	    queue[k] = temp;
	}
	k = current;
    }
}

