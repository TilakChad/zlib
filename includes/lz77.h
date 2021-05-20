#ifndef LZ77_H
#define LZ77_H

#include "stream.h"
#include "hash_table.h"

// It is the struct that will be used by/ or return by lz77 compression
// algorithm.
// distance referes to the go back distance in the output stream and length is the number of bytes that need to be copied from that position to the current output position 
typedef struct length_distance
{
    int32_t distance;
    int16_t length;
} length_distance;

// It will return the total characters it read recently and modify the length_distance* 
int lz77(stream*,sliding_window*, hash_entry**, length_distance* );

#endif

