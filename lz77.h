#ifndef LZ77_H
#define LZ77_H

#include "stream.h"
#include "hash_table.h"

typedef struct length_distance
{
    int32_t distance;
    int16_t length;
} length_distance;

// It will return the total characters it read recently and modify the length_distance* 
int lz77(stream*,sliding_window*, hash_entry**, length_distance* );

#endif

