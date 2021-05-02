// This program will implement the LZ77 variant algorithm along with the sliding windows
#include "stream.h"
#include "hash_table.h"
#include <stdio.h>
#include <assert.h>

typedef struct length_distance
{
    int32_t distance;
    int16_t length;
} length_distance;

int lz77(stream* input_stream,stream* output_stream, sliding_window* window, hash_entry** hash_table)
{
    hash_entry in_str;
    int8_t count = get_next3_bytes(input_stream, &in_str);

    input_stream->pos += count;
    update_sliding_window(input_stream, window);

    // Match will be done on the part exclding the 3 bytes
    int32_t matched_length;

    if (count == 0)
	return 0;
    
    if (count == 3)
    {
	// Implement the matching and find the largest subsequence that match the input sequence
	length_distance matched;
	matched.distance = -1;
	matched.length = -1;

	int16_t index = hash_function(in_str);
	// will check the chain of the hash table until the hash_entry is found
	hash_entry* traverser = hash_table[index];
	hash_entry* previous = hash_table[index];
	
	while(1)
	{
	    int16_t this_matched_length = 3;
	    int16_t this_distance = 0;
	    
	    if (!traverser)
		break;

	    if ( (cmp_hash_entry(in_str,*traverser)==1) &&
		 (traverser->loc >= window->start_pos && traverser->loc <= window->end_pos))
	    {
		// match the upcoming length with the input stream
		/* fprintf(stderr, "Found the location %d.\n", traverser->loc); */
		while( (input_stream->pos + this_matched_length < input_stream->len))
		{
		    if (this_matched_length >= 285)
			break;

		    if (input_stream->buffer[input_stream->pos - 3 + this_matched_length] == input_stream->buffer[traverser->loc + this_matched_length])
			this_matched_length++;
		    else
			break;
		};

		if (this_matched_length > matched.length)
		{
		    matched.length = this_matched_length;
		    matched.distance = input_stream->pos - 3  - traverser->loc;
		}
	    }

	    if ( (traverser->loc < window->start_pos) || (traverser->loc > window->end_pos))
	    {
		previous->next = NULL;
		delete_chain(traverser);
		break;
	    }

	    previous = traverser;
	    traverser = traverser->next;
	};

	// Regardless of the result add input string to the hash table
	in_str.loc = input_stream->pos - count;
	in_str.next = NULL;
	insert_table(hash_table, in_str);

	if (matched.distance == -1 && matched.length == -1)
	{
	    fprintf(stdout,"Not input symbol found -> %c%c%c.\n",in_str.str[0],in_str.str[1],in_str.str[2]);
	    for (int i = 0; i < 3; ++i)
		output_stream->buffer[output_stream->pos++] = in_str.str[i];

	    output_stream->buffer[output_stream->pos] = '\0';
	    
	    // wrtie next 3 bytes to ouput stream
	}
	else
	{
	    fprintf(stdout,"Run length : distance -> %d && length -> %d.\n",matched.distance,matched.length);
	    assert(matched.length!=-11);
	    input_stream->pos+= matched.length-3;

	    int32_t back_pos = output_stream->pos - matched.distance;
	    int32_t length = matched.length;
	    while(length--)
	    {
		output_stream->buffer[output_stream->pos++] = output_stream->buffer[back_pos++];
	    }
	    output_stream->buffer[output_stream->pos] = '\0';
	}
    }
    else
    {
	// Write count bytes to output stream.
	
    }
    return -1;
}
