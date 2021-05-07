#ifndef BINARY_IO_H
#define BINARY_IO_H
#include "inflate.h"

void write_header(stream*);

void write_deflate_header(bit_writer*);

compress_info* write_run_length(bit_writer *, int, int, int32_t *, int);

// data is the bit to be written to output stream, len is the bit to be used to write data and reverse is set if the bit are to be reversed within len before writing to the output stream

int write_bit(bit_writer*, int data, int len, bool reverse);

// This function reverses the bit pattern that is to be written as compressed
// format

int reverse_bit_pattern(int data, int len);



#endif
