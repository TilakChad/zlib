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

extern int compress(stream*, stream*, sliding_window*, hash_entry**);
#define SIZE (1024*1024)


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

    hash_entry** hash_table = init_hash_table();
    
    sliding_window window;
    window.start_pos = 0;
    window.end_pos = 0;

    // instead of writing to the main file, lets just write to outstream for now 
    compress(&instream,&outstream,&window,hash_table);


    //cleanup section 
    free(instream.buffer);
    free(outstream.buffer);
    cleanup_hash(hash_table);
    
    fclose(fp);
    return 0;   
}
