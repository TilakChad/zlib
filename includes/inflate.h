#ifndef INFLATE_H
#define INFLATE_H
#include <stdint.h>
#include "stream.h"

typedef struct compress_info
{
    int value; // To determine the code that is being coded
    int huffman_code; // Actual huffman code that is going to be written on compressed block 
    int code_length; // The length of hufman code to be written -> for eg : huffman_code = 5 & code_length = 7 would result 0000101 to be written but as reversed 
} compress_info ;

int construct_huffman_code(compress_info* , int32_t* , int );
void write_adler32(stream*, stream *);

#endif
