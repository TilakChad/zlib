#include "stream.h"
#include "hash_table.h"
#include <assert.h>
#include "lz77.h"
#include <stdio.h>
#include "huffman_tree.h"

#define NLIT 286 // 0-285 Total number of literals used by dynamic huffman code
#define NDIST 30 // 0-29  Total distance codes used by lz77

// I guess , we first need to count the total occurence of each literal .. So we make an linear array of size nlit + ndist
// hlit and hdist as specified in deflate rfc are assumed to be all that is : hlit -> 5 bits -> 32

struct run_length
{
    int code_length;
    int code_info;
    int count;
};

int16_t literal_for_length(int16_t);
int16_t literal_for_distance(int32_t);
extern void write_header(stream*);

struct run_length run_length_encoder(int32_t* arr, int cur_index, int max_index, struct run_length prev_code); 
extern void write_run_length(bit_writer*,int, int, int32_t*,int);


int compress(stream *input_stream, stream *output_stream,sliding_window *window, hash_entry **hash_table)
{
    int32_t literals[NLIT+NDIST];
    for (int i = 0; i < NLIT+NDIST; ++i)
	literals[i] = 0;

    length_distance record = (length_distance) {-1,-1};
    literals[256] = 1;
    int8_t count;
    while(1)
    {
	count = lz77(input_stream, window, hash_table, &record);
	if (!count)
	    break;
	
	if (count < 3)
	{
	    for(int i = 0; i < count; ++i)
	    {
		int16_t index = -1;
		index = input_stream->buffer[input_stream->pos - count + i];
		literals[index]++;
	    }
	    break;
	}

	// It is the case when count equals 3

	if (record.length == -1 && record.distance == -1)
	{
	    for (int i = 0; i < 3; ++i)
		literals[input_stream->buffer[input_stream->pos - count + i]]++;
	}
	else
	{
	    // Length code
	    int16_t index = literal_for_length(record.length);
	    literals[index]++;

	    // Increase the input stream by matched length
	    input_stream->pos += record.length - 3;
	    
	    // Distance code
	    index = literal_for_distance(record.distance);
	    literals[NLIT + index]++;
	}
    }
    for (int i = 0; i < NLIT + NDIST; ++i)
    {
	if (literals[i]!=0)
	    printf(" [%d] -> [%d]\n ",i,literals[i]);
    }
    // Now need to generate dynamic huffman codes form the given table ..
    // Need to count the code length of first 285+1 literals
    int32_t count_code_length[NDIST+NLIT];
    for (int i = 0; i < NDIST + NLIT; ++i)
	count_code_length[i] = 0;

    huffman_coding(literals, NLIT, count_code_length, NLIT);

    for(int i = 0; i < NLIT; ++i)
    {
	if (count_code_length[i]!=0)
	    printf(" [%d] -> [%d]\n", i , count_code_length[i]);
    }

    // Form the code lengths for the remaining distance codes also
    huffman_coding(literals+NLIT, NDIST, count_code_length+NLIT, NDIST);
    
    // This code calculates the run length of the remaining huffman codes

    struct run_length prev_code = {-1,-1,0};
    struct run_length cur_code;

    int32_t run_length_coderepeat[19];
    for (int i = 0; i < 19; ++i)
	run_length_coderepeat[i] = 0;
    
    int cur_index = 0;
    while(1)
    {
    	cur_code = run_length_encoder(count_code_length, cur_index,NLIT+NDIST , prev_code);
    	if (cur_code.count == 0 && cur_code.code_length == -1)
    	    break;

    	assert(cur_code.code_length < 19);
	cur_index += cur_code.count;
	prev_code = cur_code;
    	run_length_coderepeat[cur_code.code_length]++;
	printf("\nOutput code was %d with info %d.",cur_code.code_length,cur_code.code_info);
    };

    // Now pass it to the huffman coder and get the final code lengths
    int32_t run_length_code_length[19];
    for (int i = 0; i < 19; ++i)
	run_length_code_length[i] = 0;

    huffman_coding(run_length_coderepeat, 19, run_length_code_length, 19);

    printf("\nRun length info -> \n");
    for (int i = 0; i < 19; ++i)
    {
	if (run_length_code_length[i]!=0)
	    printf("\n [%d] -> [%d].",i, run_length_code_length[i]);
    }
    putchar('\n');

    // Now need to write to the files as compressed block .. it is the final stage
    // Initialize the bit writer
    bit_writer write_state;
    write_state.outstream = output_stream;
    write_state.bit_buffer = 0;
    write_state.count = 0;
    // begin writing to the files
    write_header(output_stream); // Writes the zlib header
    write_run_length(&write_state, NLIT, NDIST, run_length_code_length, 19);
    // Now comes the deflate compression
    

    
}



// copy paste from rfc
int16_t literal_for_length(int16_t length)
{
    if ( length >= 3 && length <=10)
	return length-3+257;

    if (length == 11 || length == 12)
	return 265;

    if (length == 13 || length == 14)
	return 266;

    if (length == 15 || length == 16)
	return 267;

    if (length == 17 || length == 18)
	return 268;

    if (length >= 19 && length <= 22)
	return 269;

    if (length >= 23 && length <= 26)
	return 270;

    if (length >= 27 && length<=30)
	return 271;

    if (length >= 31 && length <= 34)
	return 272;

    if (length >= 35 && length <= 42)
	return 273;

    if (length >= 43 && length <= 50)
	return 274;

    if (length >= 51 && length <= 58)
	return 275;

    if (length >= 59 && length <= 66)
	return 276;

    if (length >= 67 && length <= 82)
	return 277;

    if(length >= 83 && length <= 98)
	return 278;

    if (length >=99 && length <= 114)
	return 279;

    if (length >= 115 && length <= 130)
	return 280;

    if (length >= 131 && length <= 162)
	return 281;

    if (length >= 163 && length <= 194)
	return 282;

    if (length >= 195 && length <= 226)
	return 283;

    if (length >= 227 && length <= 257)
	return 284;

    if (length == 285)
	return 258;

    assert(!"Unknown length");
}

int16_t literal_for_distance(int32_t dist)
{
    assert(dist!=0);
    if (dist < 5)
	return dist-1;

    if (dist == 5|| dist == 6)
	return 4;

    if (dist == 7 || dist == 8)
	return 5;

    if (dist >= 9 && dist <= 12)
	return 6;
    
    if (dist >= 13 && dist <= 16)
	return 7;

    if (dist >= 17 && dist <= 24)
	return 8;

    if (dist >= 25 && dist <= 32)
	return 9;

    if (dist >= 33 && dist <= 48)
	return 10;

    if (dist >= 49 && dist <= 64)
	return 11;

    if (dist >= 65 && dist <= 96)
	return 12;

    if (dist >= 97 && dist <= 128)
	return 13;

    if (dist >= 129 && dist <= 192)
	return 14;

    if (dist >= 193 && dist <= 256)
	return 15;

    if (dist >= 257 && dist <= 384)
	return 16;

    if (dist >= 385 && dist <= 512)
	return 17;

    if (dist >= 513 && dist <= 768)
	return 18;

    if (dist >= 769 && dist <= 1024)
	return 19;

    if (dist >= 1025 && dist <= 1536)
	return 20;

    if (dist >= 1537 && dist <= 2048)
	return 21;

    if (dist >= 2049 && dist <= 3072)
	return 22;

    if (dist >= 3073 && dist <= 4096)
	return 23;

    if (dist >= 4097 && dist <= 6144)
	return 24;

    if (dist >= 6145 && dist <= 8192)
	return 25;

    if (dist >= 8193 && dist <= 12288)
	return 26;

    if (dist >= 12289 && dist <= 16384)
	return 27;

    if (dist >= 16385 && dist <= 24576)
	return 28;

    if (dist <= 24577 && dist <= 32768)
	return 29;

    assert(!"Not a valid distance code...");
	    
}


struct run_length run_length_encoder(int32_t* code_lengths, int cur_index, int max_index, struct run_length prev_code)
{
    if (cur_index>=max_index)
    {
	return (struct run_length){-1,-1,0};
    }

    struct run_length temp = {-1,-1,0};
    
    if (code_lengths[cur_index] >= 0 && code_lengths[cur_index] <= 15)
    {

	if (code_lengths[cur_index] >= 1 && code_lengths[cur_index] <= 15)
	{
	    int count = 0;
	    while(cur_index<max_index && count < 6)
	    {
		if (code_lengths[cur_index] == prev_code.code_length)
		{
		    cur_index++;
		    ++count;
		}
		else
		    break;
	    }
	    if (count > 2)
	    {
		temp.code_length = 16;
		temp.code_info = count;
		temp.count = count;
		return temp;
	    }
	    else
	    {
		temp.code_length = code_lengths[cur_index-count];
		temp.count = 1;
		return temp;
	    }
	}
	else
	{
	    int count_zero = 0;
	    while (cur_index < max_index && count_zero < 138)
	    {
		if (code_lengths[cur_index] == 0)
		{
		    cur_index++;
		    count_zero++;
		}
		else
		    break;
	    }
	    if (count_zero < 3)
	    {
		temp.code_length = 0;
		temp.count = 1;
		return temp;
	    }
	    else
	    {
		if (count_zero > 10 && count_zero < 139)
		{
		    temp.code_length = 18;
		    temp.code_info = count_zero;
		    temp.count = count_zero;
		}
		else if (count_zero < 11)
		{
		    temp.code_info = 17;
		    temp.code_info = count_zero;
		    temp.count = count_zero;
		}
		else
		{
		    temp.code_info = -1;
		    temp.code_length = -1;
		    assert(!"Invalid amounts of zero .. ");
		}
		return temp;
	    }
	}
    }
    return temp;
}
	    
