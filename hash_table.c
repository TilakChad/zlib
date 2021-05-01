#include "hash_table.h"

hash_entry** init_hash_table()
{
    hash_entry** hash_table = malloc(sizeof (hash_entry*) * HASH_TABLE_SIZE);

    for(int i = 0; i < HASH_TABLE_SIZE; ++i)
	hash_table[i] = NULL;

    return hash_table;
}

int16_t hash_function(hash_entry 
