#include "priority_minheap.h"
#include <stdlib.h>

int build_huffman_tree(priority_minheap*);

void print_tree(node*, char* str, int32_t index);

int main()
{
    printf("Enter size ");
    uint16_t size;
    scanf("%hd",&size);
    
    priority_minheap minheap = init_minheap(size);
    printf("\nEnter name and freq : \n");
    for (int i = 0; i < size; ++i)
    {
	node* newnode = malloc(sizeof(node));
	newnode->left = NULL;
	newnode->right = NULL;
	scanf("%hd",&newnode->value);
	scanf("%hd",&newnode->frequency);
	/* newnode->frequency = i; */
	insert_pqueue(&minheap, newnode);
    }
    build_huffman_tree(&minheap);

    /* while(minheap.current_index!=0) */
    /* { */
    /* 	node* min = remove_min(&minheap); */
    /* 	printf(" %c -> %hd.\n",min->value,min->frequency); */
    /* }; */
    
    char buffer[100];
    print_tree(minheap.queue[0],buffer,0);
    
    return 0;
}

int build_huffman_tree(priority_minheap* pqueue)
{
    while(pqueue->current_index!=1)
    {
	node* node1 = remove_min(pqueue);
	if(!node1)
	{
	    printf("\nsomething's wrong...with node 1");
	    return -1;
	}
	node* node2 = remove_min(pqueue);
	if (!node2)
	{
	    printf("Something's wrong with node 2\n");
	    return -2;
	}

	node* left = node2;
	node* right = node1;
	if (node1->frequency <= node2->frequency)
	{
	    left = node1;
	    right = node2;
	}

	// Combine into one node
	node* newnode = malloc(sizeof(node));
	newnode->left = left;
	newnode->right = right;
	newnode->value = -1;
	newnode->frequency = left->frequency + right->frequency;
	printf("%c %c combined to -> %hd.\n",left->value,right->value,newnode->frequency);
	insert_pqueue(pqueue, newnode);
    }
    return 0;
}

void print_tree(node* tree, char* str, int32_t index)
{
    if (!tree)
    {
	str[index] = '\0';
	return;
    }
    
    if (tree->left != NULL)
    {
	str[index] = '0';
	print_tree(tree->left, str, index+1);
    }
    if (tree->right != NULL)
    {
	str[index] = '1';
	print_tree(tree->right, str, index+1);
    }

    if (tree->left == NULL && tree->right == NULL)
    {
	str[index]='\0';
	printf("\n%c -> %s.",tree->value ,str);
    }
}
    
    
    
