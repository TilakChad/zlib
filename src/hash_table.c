#include "hash_table.h"
#include "stream.h"

hash_entry** init_hash_table()
{
    hash_entry** hash_table = malloc(sizeof (hash_entry*) * HASH_TABLE_SIZE);

    for(int i = 0; i < HASH_TABLE_SIZE; ++i)
	hash_table[i] = NULL;

    return hash_table;
}

// Not a fancy function .. returns the middle byte
// Could be made much much larger for space time tradeoff 
int16_t hash_function(hash_entry entry)
{
    return entry.str[1];
}

bool cmp_hash_entry(hash_entry a, hash_entry b)
{
    return (a.str[0] == b.str[0]) &&
	(a.str[1] == b.str[1]) &&
	(a.str[2] == b.str[2]);
}

int insert_table(hash_entry** hash_table, hash_entry entry)
{
    int16_t index = hash_function(entry);

    if (!hash_table[index])
    {
	hash_table[index] = malloc(sizeof(hash_entry));
	if(!hash_table[index])
	    return -1;
	
	hash_table[index]->next = NULL;
	hash_table[index]->str[0] = entry.str[0];
	hash_table[index]->str[1] = entry.str[1];
	hash_table[index]->str[2] = entry.str[2];
	hash_table[index]->loc = entry.loc;
	return 0;
    }

    hash_entry* head = hash_table[index];
    hash_table[index] = malloc(sizeof(hash_entry));
    hash_table[index]->next = head;
    hash_table[index]->loc = entry.loc;
    hash_table[index]->str[0] = entry.str[0];
    hash_table[index]->str[1] = entry.str[1];
    hash_table[index]->str[2] = entry.str[2];
    return 0;
}

int delete_chain(hash_entry* node)
{
    if (!node) 
	return 0;
    delete_chain(node->next);
    node->next = NULL;
    free(node);
    return 0;
}

int16_t get_next3_bytes(stream* input_stream, hash_entry* container)
{
    int16_t count = 0;
    container->loc = input_stream->pos;

    int32_t current_pos = input_stream->pos;
    
    while( (current_pos < input_stream->len) && (count < 3))
    {
	container->str[count++] = input_stream->buffer[current_pos++];
    }

    return count; 
}

void update_sliding_window(stream* input_stream, sliding_window* window)
{
    window->end_pos = input_stream->pos;

    if ((window->end_pos - window->start_pos) >= 32 * 1024)
    {
	int32_t diff = (window->end_pos - window->start_pos) - 32 * 1024;
	window->start_pos += diff;
    }
}

void cleanup_hash(hash_entry** hash_table)
{
    for (int i = 0; i < HASH_TABLE_SIZE; ++i)
    {
	if(hash_table[i] != NULL)
	    delete_chain(hash_table[i]);
    }
    free(hash_table);
}
