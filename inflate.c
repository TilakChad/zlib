
// This function should take a struct which should contain the code, its huffman_code and its huffman encoded length
// This function is partially completed and need to modify it
#include "inflate.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int construct_huffman_code(compress_info* info_table, int32_t* code_length, int size)
{
    assert(size!=0);
    int max_code_length = code_length[0];
    for (int i = 0; i < size; ++i)
    {
	if (max_code_length < code_length[i])
	    max_code_length = code_length[i];
    }

    // Count numbers of each code_length
    int* each_code_count = malloc(sizeof(int) * (max_code_length+1));
    for (int i = 0; i < max_code_length + 1; ++i)
	each_code_count[i] = 0;

    for (int i = 0; i < size; ++i)
	each_code_count[code_length[i]]++;

    // Need offset array to keep count of each code length offset
    for (int i = 0; i < max_code_length+1; ++i)
	printf("\nCount [%d] -> %d.",i,each_code_count[i]);
    
    int* offset = malloc(sizeof(int) * (max_code_length+1));

    offset[0] = 0;
    each_code_count[0] = 0;
    for (int i = 1; i < max_code_length + 1; ++i)
    {
	offset[i] = offset[i-1] + each_code_count[i-1];
	offset[i] <<=1;
    }

    for (int i = 0; i < size; ++i)
    {
	info_table[i].value = i;
	info_table[i].code_length = code_length[i];
	info_table[i].huffman_code = offset[code_length[i]]++;
    }
    return 0;
}

