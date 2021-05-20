// First we will start with hash_table that stores 3 consecutive values along
// with the position that they appeared within input stream.
// We will implement lz77 compression technique to produce run length encoding
// Length of each huffman symbol need to be find using priority minheap and
// huffman tree We already have dynamic huffman encoding
// Bitpack at last

// At least 3 bytes should be matched for using lz77 compression technique;
#include "stream.h"
#include <stdio.h>
#include "hash_table.h"
#include "binary_IO.h"

extern int compress(stream*,bit_writer*, sliding_window*, hash_entry**,int*);

#define SIZE (1024*1024*25) // Max input size .. currently 25 MB. Can be changed as desired

void display_progress_bar(stream*);

int main(int argc, char **argv)
{
    if(argc<3)
    {
	fprintf(stderr,"\nNot enough arguments.. It should be \n ./exe_name in_file comp_file");
	return -1;
    }
    stream instream;
    stream outstream;

    outstream.buffer = malloc(sizeof(unsigned char) * SIZE);
    outstream.len = SIZE;

    outstream.pos = 0;

    FILE* fp = fopen(argv[1],"rb");
    if (!fp)
    {
	fprintf(stderr,"Failed to open %s.\n",argv[1]);
	return -1;
    }
    
    instream.buffer = malloc(sizeof(unsigned char) * SIZE);
    
    uint32_t read_size;
    read_size = fread(instream.buffer,sizeof(unsigned char), SIZE,fp);
    
    fprintf(stderr,"Total read bytes were : %d.\n\n\n\n",read_size);
    
    instream.pos = 0;
    instream.len = read_size;
    bit_writer writes_bits;
    writes_bits.bit_buffer = 0;
    writes_bits.outstream = &outstream;
    writes_bits.count = 0;
    
    hash_entry** hash_table;
    sliding_window window;

    // writes the first 2 bytes of the zlib header required 
    write_zlib_header(&writes_bits);

    int block_count = 0;
    int literal_count = 0;
    while(1)
    {
	literal_count = 0;

	// Set the sliding window position taking current input position as reference 
	window.start_pos = instream.pos;
	window.end_pos = instream.pos;

	// New hash table is created for each block and destroyed at the end of that block 
        hash_table = init_hash_table();

	++block_count;
	display_progress_bar(&instream);
	
	// instead of writing to the main file, lets just write to outstream for now 
	compress(&instream,&writes_bits, &window,hash_table,&literal_count);
	
	if(literal_count<MAX_LITERAL_PER_BLOCK)
	{
	    // It is the last block.. No more literals remained
	    writes_bits.outstream->buffer[writes_bits.outstream->pos++] = writes_bits.bit_buffer;
	    write_adler32(&instream,&writes_bits);
	    cleanup_hash(hash_table);
	    break;
	}
	cleanup_hash(hash_table);
    }

    // Write the output data to the file given in command line
    FILE* compressed = fopen(argv[2],"wb");
    if(!compressed)
    {
    	fprintf(stderr,"Error in opening compressed.zlib for writing...");
    	return 1;
    }
    
    fwrite(writes_bits.outstream->buffer,sizeof(unsigned char),writes_bits.outstream->pos,compressed);
    fclose(compressed);
    //cleanup section 
    free(instream.buffer);
    free(outstream.buffer);

    fprintf(stderr,"\nTotal block count were -> %d.",block_count);
    fprintf(stderr,"\nTotal input written were -> %d.",instream.pos);
    fclose(fp);
    return 0;   
}

void display_progress_bar(stream* in_stream)
{
    // calculates then total fraction of the input consumes
    
    float fraction = in_stream->pos / (float) in_stream->len;
    int percent = fraction * 100;
    int i = 0;
    #ifdef __linux__
    printf("\e[3A\nCompressing : ");
    #endif
    putchar('\n');
    for (int i = 0; i < percent; ++i)
    {
	printf("#");
    }
    for (int i = percent; i < 100; ++i)
    {
	printf("-");
    }
    printf(" [%d]%%\n",percent);
}
