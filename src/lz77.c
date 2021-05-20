// This program will implement the LZ77 variant algorithm along with the sliding windows
#include "stream.h"
#include "hash_table.h"
#include <stdio.h>
#include <assert.h>
#include "lz77.h"

// This function returns the longest possible match of the next 3 bytes from
// input stream within the sliding window.
// If there are less than 3 bytes available in input stream, it returns with
// count < 3;
// It traverses the chain(linked list) of the hash_entry and determines the max
// possible length within 32 k sliding window.
// New entries of 3 bytes are pushed as a hash_entry in hash_table.
// For equal length matched, the nearest (lesser distance) is choosen.
// For making memeory efficient, hash_entry that goes out of sliding_window (i.e hash_entry->loc not within sliding window)are
// deallocated dynamically.

int lz77(stream* input_stream, sliding_window* window, hash_entry** hash_table, length_distance* record)
{
    hash_entry in_str;
    int8_t count = get_next3_bytes(input_stream, &in_str);

    int match_started_index = input_stream->pos;
    
    input_stream->pos += count;
    update_sliding_window(input_stream, window);

    // Match will be done on the part exclding the 3 bytes
    int32_t matched_length;

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
		    if (traverser->loc + this_matched_length >= match_started_index)
			break;
		    if (this_matched_length >= 258)
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
		    // if (matched.distance > 32000)
			// printf("\nMatched distance is %d.",matched.distance);
			
		}
	    }

	    // Deallocate the chain that goes out of sliding_window
	    // It is safe to delete chain since new entries are added at the front of the chain. So, entry after that are also out of sliding window.
	    if ( (traverser->loc < window->start_pos) || (traverser->loc > window->end_pos))
	    {

		if (traverser == hash_table[index])
		{
		    delete_chain(hash_table[index]);
		    hash_table[index] = NULL;
		    break;
		}
       		delete_chain(traverser);
		previous->next = NULL;
		break;
	    }

	    // Continue traversing the chain 
	    previous = traverser;
	    traverser = traverser->next;
	};

	// Regardless of the result add input string to the hash table
	in_str.loc = input_stream->pos - count;
	in_str.next = NULL;
	insert_table(hash_table, in_str);

	record->length = matched.length;
	record->distance = matched.distance;

    }
    else
    {
	record->length = -1;
	record->distance = -1;
    }

    return count;
}
