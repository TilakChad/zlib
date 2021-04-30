#ifndef STREAM_H
#define STREAM_H

#include <stdint.h>

// This header file will be used for denoting both input stream as well as output stream
// Input stream is the one that read from the uncompressed file and applies certain transformations
// Output stream writes the compressed data into respective files after succesful compression

// So what should a stream have?
// We will read all files at once ..
// -> It should have a buffer to store the read data.
// -> It should have a pointer to indicate where its reading head is (for input stream) and writing head is (for output stream)
// -> It should have total length allowed to be read or written as input or output respectively.

struct stream
{
    unsigned char* buffer; // Since we will be going to operate on a byte level
    int32_t pos; // Current pos of the stream
    int32_t len; // Total length of the stream
};

#endif
// Lets leave it at this for now...
