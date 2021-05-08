#include <stdio.h>
#include "stream.h"
#include <stdbool.h>
#include "inflate.h"
#include <stdlib.h>
#include <assert.h>
#include "binary_IO.h"

void write_deflate_header(bit_writer* writer, int bfinal, int btype)
{
    write_bit(writer, bfinal, 1,false);
    write_bit(writer, btype, 2,false);
}

void write_zlib_header(bit_writer* writer)
{
    // First bytes of the zlib header contains
    /*
    	    +-------------------------------+
	    |    CMF  	   |   FLG     	    |
	    +-------------------------------+
	    
	    CMF -> bits 0 to 3 CM (Compression method)
	        -> bits 4 to 7 CINFO (Compression info)
    */
    writer->outstream->pos = 0;

    writer->outstream->buffer[0] = (writer->outstream->buffer[0] & 0xF0) | 0x08;
    writer->outstream->buffer[0] = (writer->outstream->buffer[0] & 0x0F) | 0x70;

    // Setting FDICT = 0
    writer->outstream->buffer[1] = 0x00;
    // Setting compression level to default
    writer->outstream->buffer[1] = 0x80;

    // Finally write the value of FCHECK
    // could've easily hardcoded but lets leave this for now 
    uint32_t num = writer->outstream->buffer[0] * 256;
    num = (num % 31 + writer->outstream->buffer[1] % 31)%31;
    num = 31- num;
    writer->outstream->buffer[1] += num;

    // Writ the run length_encoding now
    writer->outstream->pos+=2; 
}


compress_info* write_run_length(bit_writer* bit_state, int nlit, int ndist, int32_t* run_length_count, int run_length_size)
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
    int hclen = 19 - count_zero - 4;
    int hdist = ndist - 1;
    int hlit = nlit - 257;
    write_bit(bit_state, hlit, 5, false);
    write_bit(bit_state, hdist, 5, false);
    write_bit(bit_state, hclen, 4, false);

    // Now I need to wrtie a procedure that converts code lengths into huffman code
    compress_info* run_lengths = malloc(sizeof(compress_info) * 19);
    
    for (int i = 0; i < 19; ++i)
    {
	run_lengths[i].value = i;
	run_lengths[i].code_length = 0;
	run_lengths[i].huffman_code = 0;
    }

    construct_huffman_code(run_lengths, run_length_count, 19);

    for (int i = 0; i < 19; ++i)
    {
	if (run_lengths[i].code_length!=0)
	    printf("\nRun length are :-> Value : %d, code_length = %d and code : %d.",run_lengths[i].value,run_lengths[i].code_length,run_lengths[i].huffman_code);
    }

    //Before continuing, let's check the state of the bit writer
    printf("\n----------------------- Bit writer ----------------------------\n");
    printf("Bitwriter->buffer pos -> %d. \n bitcount -> %d. \n Current buffer %d.\n",bit_state->outstream->pos, bit_state->count, bit_state->bit_buffer);
    // Let's continue
    // OK. This procedure right the length of the code lengths that is to be built by decompressor while decoding further 
    for (int i = 0; i < hclen + 4; ++i)
    {
	    write_bit(bit_state, run_lengths[expected_order[i]].code_length, 3, false);
	    printf("\n Writing -> %d -> %d.",expected_order[i], run_lengths[expected_order[i]].code_length);
    }
    return run_lengths;
    
}

int write_bit(bit_writer* bit_state, int data, int len, bool reverse)
{
    if (len == 0)
	return 0;
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
