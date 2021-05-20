
// This function should take a struct which should contain the code, its huffman_code and its huffman encoded length
// This function is partially completed and need to modify it
#include "inflate.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int construct_huffman_code(compress_info* info_table, int32_t* code_length, int size)
{
    // Forms the cannonical huffman code
    // Huffman code used by deflate depend only upon the code length
    // Code length information is enough to produce unambigious huffman code
    
    assert(size!=0);
    int max_code_length = code_length[0];
    for (int i = 0; i < size; ++i)
    {
	if (max_code_length < code_length[i])
	    max_code_length = code_length[i];
    }

    assert(max_code_length<=15);

    // Count numbers of each code_length
    int* each_code_count = malloc(sizeof(int) * (max_code_length+1));
    for (int i = 0; i < max_code_length + 1; ++i)
	each_code_count[i] = 0;

    for (int i = 0; i < size; ++i)
	each_code_count[code_length[i]]++;

    // Need offset array to keep count of each code length offset
    
    int* offset = malloc(sizeof(int) * (max_code_length+1));

    offset[0] = 0;
    each_code_count[0] = 0;
    // my slight variation of huffman code
    // Direct function provided in deflate rfc may be used .. they are equivalent
    for (int i = 1; i < max_code_length + 1; ++i)
    {
	offset[i] = offset[i-1] + each_code_count[i-1];
	offset[i] <<=1;
    }

    // Passed array modified 
    for (int i = 0; i < size; ++i)
    {
	info_table[i].value = i;
	info_table[i].code_length = code_length[i];
	info_table[i].huffman_code = offset[code_length[i]]++;
    }
    free(each_code_count);
    free(offset);
    return 0;
}

void write_adler32(stream* instream, bit_writer* bit_write)
{
    if (bit_write->outstream->pos > bit_write->outstream->len - 4)
    {
	fprintf(stderr,"Output buffer not big enough to write adler32 checksum\n");
	return ;
    }

    // Write the 32 bit adler32 checksum in network order
    uint16_t a = 1;
    uint16_t b = 0;
    for (int i = 0; i < instream->len; ++i)
    {
	a = (a + instream->buffer[i])%65521;
	b = (b + a) % 65521;
    }
    uint32_t adler32 = a << 16 + b;

    // write adler32 to outstream 
    for (int i = 0; i < 4; ++i)
    {
	bit_write->outstream->buffer[bit_write->outstream->pos++] = (adler32 >> ((3-i)*8)) & 0xFF ; 
    }   
}
