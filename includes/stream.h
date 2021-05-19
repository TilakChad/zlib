#ifndef STREAM_H
#define STREAM_H

#include <stdint.h>

// This header file will be used for denoting both input stream as well as output stream
// Input stream is the one that read from the uncompressed file and applies certain transformations
// Output stream writes the compressed data into respective files after succesful compression

// So what should a stream have?
// We will read all files at once ..
// -> It should have a buffer to store the read data.
// -> It should have a pointer to indicate where its reading head is (for input stream) or writing head is (for output stream)
// -> It should have total length allowed to be read or written as input or output respectively.

typedef struct stream
{
    unsigned char* buffer; // Since we will be going to operate on a byte level
    int32_t pos; // Current pos of the stream
    int32_t len; // Total length of the stream
} stream;

// Lets implement the sliding window too

typedef struct sliding_window
{
    int32_t start_pos; // start of the sliding window (pos) relative to the input stream
    int32_t end_pos; // size of the sliding window is 32K .. so max(end_pos-start_pos) < 32 * 1024;
}sliding_window;

// Modify the sliding window depending upon the input stream reading 
void update_sliding_window(stream*, sliding_window *); 

typedef struct bit_writer
{
    stream* outstream;
    int32_t bit_buffer;
    int32_t count;
} bit_writer;

#define MAX_LITERAL_PER_BLOCK 16384

#endif
// Lets leave it at this for now...
