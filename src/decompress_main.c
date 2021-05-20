#include <stdio.h>
#include <stdlib.h>

extern unsigned char *zlib_stream_decompress(unsigned char *data, int len,
                                             int *outlen);

#define MAX_INPUT_SIZE (50 * 1024 * 1024) // Currently set to 50 MB .. Set however you need . This could be made dynamic that allocates space auto though .. 

int main(int argc, char **argv)
{
    if (argc < 3)
    {
	fprintf(stderr, "Not enough arguments.. It should be \n ./exe_name comp_file out_file");
	return -1;              
    }

    FILE* in_file;
    if ( !(in_file = fopen(argv[1],"rb")))
    {
	fprintf(stderr,"Failed to open input file %s. Exiting ... ",argv[1]);
	return -2;
    }

    unsigned char* in_buffer = malloc(sizeof(unsigned char) * MAX_INPUT_SIZE);

    int read_size = fread(in_buffer,sizeof(unsigned char),MAX_INPUT_SIZE,in_file);

    int out_len = 0;
    
    unsigned char* out_buffer = zlib_stream_decompress(in_buffer,read_size,&out_len);
    fclose(in_file);

    FILE* out_file;
    if (!(out_file = fopen(argv[2],"wb")))
    {
	fprintf(stderr,"Failed to open output file %s for writing. Exiting",argv[2]);
	return -3;
    }
    
    fwrite(out_buffer,sizeof(unsigned char), out_len, out_file);
    fclose(out_file);

    free(in_buffer);
    free(out_buffer);
    return 0;
}
