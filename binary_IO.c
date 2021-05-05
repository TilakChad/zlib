#include <stdio.h>
#include "stream.h"
#include <stdbool.h>

void write_header(stream*);
void write_run_length(bit_writer *, int, int, int32_t *, int);

// data is the bit to be written to output stream, len is the bit to be used to write data and reverse is set if the bit are to be reversed within len before writing to the output stream

int write_bit(bit_writer*, int data, int len, bool reverse);

// This function reverses the bit pattern that is to be written as compressed
// format

int reverse_bit_pattern(int data, int len);

void write_header(stream* outstream)
{
    // First bytes of the zlib header contains
    /*
    	    +-------------------------------+
	    |    CMF  	   |   FLG     	    |
	    +-------------------------------+
	    
	    CMF -> bits 0 to 3 CM (Compression method)
	        -> bits 4 to 7 CINFO (Compression info)
    */
    outstream->pos = 0;

    outstream->buffer[0] = (outstream->buffer[0] & 0xF0) | 0x08;
    outstream->buffer[0] = (outstream->buffer[0] & 0x0F) | 0x70;

    // Setting FDICT = 0
    outstream->buffer[1] = 0x00;
    // Setting compression level to default
    outstream->buffer[1] = 0x80;

    // Finally write the value of FCHECK
    // could've easily hardcoded but lets leave this for now 
    uint32_t num = outstream->buffer[0] * 256;
    num = (num % 31 + outstream->buffer[1] % 31)%31;
    num = 31- num;
    outstream->buffer[1] += num;

    // Writ the run length_encoding now
    outstream->pos+=2; 
}


void write_run_length(bit_writer* bit_state, int nlit, int ndist, int32_t* run_length_count, int run_length_size)
{
    int expected_order[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13,2, 14, 1, 15};
    for (int i = 0; i < 19; ++i)
    {
	printf("\n[%d] -> %d.",expected_order[i], run_length_count[expected_order[i]]);
    }

    // find the no of last entries that are zero so that they can be discarded

    int count_zero = 0;
    for (int i = 18; i >= 0; --i)
    {
	if (run_length_count[expected_order[i]] == 0)
	    count_zero++;
	else
	    break;
    }
    printf("\nTotal zeroes from last were %d.\n",count_zero);
    int hclen = 19 - count_zero;
    int hdist = ndist - 1;
    int hlit = nlit - 257;
    
}

int write_bit(bit_writer* bit_state, int data, int len, bool reverse)
{
    if (reverse)
	data = reverse_bit_pattern(data, len);

    // Manage the bit buffer and the count
    data = data << bit_state->count;
    bit_state->bit_buffer |= data;
    bit_state->count += len;

    bool written = false;
    
    while(bit_state->count>=8)
    {
	if (bit_state->outstream->pos >= bit_state->outstream->len)
	    return -1;
	written = true;
	bit_state->outstream->buffer[bit_state->outstream->pos++] = bit_state->bit_buffer & 0xff;
	bit_state->count -= 8;
	bit_state->bit_buffer >>= 8;
    }

    if (written)
	return bit_state->outstream->buffer[bit_state->outstream->pos-1];
    else
	return 0;
}

int reverse_bit_pattern(int data, int len)
{
    // We need to reverse the bit pattern like if 5 occupies 00101 then it should be reversed to 10100 for compression
    // huffman code need to be bit reversed according to deflate rfc for storing so this need to

    for (int bit_pos = 0; bit_pos < len/2; ++bit_pos)
    {
	// extract the leftmost ith bit
	int32_t bit_last = (data & 1 << bit_pos) >> bit_pos;
	int32_t bit_first = (data & 1 << (len - 1 - bit_pos)) >> (len - 1 - bit_pos);

	// Bit set the corresponding bits
	// Set len-1-bit_pos bits to bit_last and bit_pos bit to bit_first

	// Set the len-1-bit_pos bit to bit_last
	data = (data & ~(bit_first << (len - 1 - bit_pos))) | (bit_last << (len-1-bit_pos));

	// same with another bits
	data = (data & ~(bit_last << bit_pos)) | (bit_first << bit_pos);
    }
    return data;
}
