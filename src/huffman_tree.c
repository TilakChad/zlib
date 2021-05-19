#include "priority_minheap.h"
#include <assert.h>
#include <stdlib.h>
#include "huffman_tree.h"
int build_huffman_tree(priority_minheap *);

int huffman_coding(int32_t* array, unsigned arr_size, int32_t* count, unsigned count_len,int max_code_length)
{
    int32_t count_non_zero = 0;

    for (unsigned i = 0; i < arr_size; ++i)
    {
	if (array[i]!=0)
	    count_non_zero++;
    }

    if(count_non_zero==0)
	return 2;
    priority_minheap minheap = init_minheap(count_non_zero);

    // printf("\nCount non zero were %d. nnd %d\n",count_non_zero , arr_size);
    for (int i = 0; i < arr_size; ++i)
    {
	if (array[i]!=0)
	{
	    node* newnode = malloc(sizeof(node));
	    newnode->left = NULL;
	    newnode->right = NULL;
	    newnode->value = i;
	    newnode->frequency = array[i];
	    /* newnode->frequency = i; */
	    insert_pqueue(&minheap, newnode);
	}
    }
    
    build_huffman_tree(&minheap);
    
    
    while(height(minheap.queue[0]) > max_code_length+1)
    {
	printf("\n\nTree rebalanced .... \n");
	rebalance_huffman_tree_2D(minheap.queue);
    }
    
    code_length(minheap.queue[0], 0, count, count_len);

    cleanup_tree(minheap.queue[0]);
    free(minheap.queue);
    
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

	insert_pqueue(pqueue, newnode);
    }
    return 0;
}

// helper function if needed 
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

void code_length(node* tree, int32_t index, int32_t* count, unsigned count_len)
{
    if (!tree)
	return;

    if (tree->left!=NULL)
	code_length(tree->left, index+1, count,count_len);

    if (tree->right!=NULL)
	code_length(tree->right, index+1, count,count_len);

    if (tree->left == NULL && tree->right == NULL)
    {
	assert(tree->value < count_len);
	count[tree->value] = index;
    }
}

void cleanup_tree(node *tree)
{
    if (!tree)
	return;

    cleanup_tree(tree->left);
    cleanup_tree(tree->right);

    free(tree);
}

int height(node* tree)
{
    if (!tree)
	return 0;

    return (height(tree->left) > height(tree->right)) ? height(tree->left) + 1 : height(tree->right) + 1;
}


// This might not work .. so let's pass multidimensional array to rebalance the tree ... code below 
/* int rebalance_huffman_tree(node* tree) */
/* { */
/*     if (!tree) */
/* 	return 0; */

/*     if (height(tree->right) - height (tree->left) == 2) */
/*     { */
/* 	// Do some rebalancing */
/* 	// do left rotation here */
	
/*     } */

/*     return rebalance_huffman_tree(tree->right); */
    
/* } */


// 2D array might work but need to remain careful

// This function balances the tree from top which isn't good for huffman code generation . So need to think of an alternative
// This functions are not deleted for reference purpose (^.^)
/* int rebalance_huffman_tree_2D(node** tree) */
/* { */
/*     if (!(*tree)) */
/* 	return 0; */

/*     if (height((*tree)->right) - height((*tree)->left) == 2) */
/*     { */
/* 	// here balancing would work .. it is analagous to passing a reference to pointer in C++.. Think nicely and you would get it */
/* 	node* temp_root = *tree; */
	
/* 	*tree = temp_root->right; */
/* 	temp_root->right = (*tree)->left; */
/* 	(*tree)->left = temp_root; */
/* 	return 0; */
/* 	// Lets hope it would work */
/*     } */
/*     return rebalance_huffman_tree_2D(&(*tree)->right); // Else continue exploring right subtree since right subtree will generally have greater hieght in comparison to left subtree in huffman_tree */
/*     // Will check it tomorrow */
/* } */


// Lol .. nice 
int rebalance_huffman_tree_2D(node** tree)
{
    if (!(*tree))
    {
	// Returning -1 here instead of returning 0 would help to perform the rotation only once .. good 
	return -1;
	
    }
    else {
	// Explore the deepest leaf first and backtrack to find the unbalanced node and perform tree rotation there
	// It will ensure that rotation is performed only once during each call so that huffman tree remains sub optimal
	// proceed toward the tree with greater height
	int return_code = -1;
	
	if (height((*tree)->left) > height((*tree)->right))
	    return_code = rebalance_huffman_tree_2D(&(*tree)->left);
	else
	    return_code = rebalance_huffman_tree_2D(&(*tree)->right);
	if (return_code != -1)
	    return return_code;
	
	if (height((*tree)->right) - height((*tree)->left) >= 2)
	{
	    // here balancing would work .. it is analagous to passing a reference to pointer in C++.. Think nicely and you would get it
	    node* temp_root = *tree;
	
	    *tree = temp_root->right;
	    temp_root->right = (*tree)->left;
	    (*tree)->left = temp_root;
	    return 0;
	    // Lets hope it would work
	}
	else if (height((*tree)->left) - height((*tree)->left) >= 2)
      	{
	    printf("\n\nLeft balancing required...");
	    // Currently halts if this condition arises. Will experiment first and I will determine if it needs to be implemented 
	    assert(!"Left balancing not implemented .. ");
	}
    }
    return -1;
}
    
