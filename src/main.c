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
#define SIZE (1024*1024*5)


int main(int argc, char **argv)
{
    if(argc<2)
    {
	fprintf(stderr,"\nNot enough arguments \n");
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

    /* FILE* op = fopen(argv[2],"wb"); */
    /* if (!op) */
    /* { */
    /* 	fprintf(stderr,"Failed to open %s for writing.\n",argv[2]); */
    /* 	return -2; */
    /* } */
    
    instream.buffer = malloc(sizeof(unsigned char) * SIZE);
    
    uint32_t read_size;
    read_size = fread(instream.buffer,sizeof(unsigned char), SIZE,fp);
    
    fprintf(stderr,"Total read bytes were : %d.\n",read_size);
    
    instream.pos = 0;
    instream.len = read_size;
    bit_writer writes_bits;
    writes_bits.bit_buffer = 0;
    writes_bits.outstream = &outstream;
    writes_bits.count = 0;
    
    hash_entry** hash_table;
    sliding_window window;

    write_zlib_header(&writes_bits);
    
    int literal_count = 0;
    while(1)
    {
	literal_count = 0;;
	window.start_pos = instream.pos;
	window.end_pos = instream.pos;

        hash_table = init_hash_table();

	
	// instead of writing to the main file, lets just write to outstream for now 
	compress(&instream,&writes_bits, &window,hash_table,&literal_count);
	if(literal_count<MAX_LITERAL_PER_BLOCK)
	{
	    writes_bits.outstream->buffer[writes_bits.outstream->pos++] = writes_bits.bit_buffer;
	    write_adler32(&instream,&writes_bits);
	    cleanup_hash(hash_table);
	    break;
	}
	cleanup_hash(hash_table);
    }

    FILE* compressed = fopen("comp.deflate","w");
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

    
    fclose(fp);
    return 0;   
}
