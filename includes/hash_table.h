#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "stream.h"
// It is the 3 byte content hash-table that will be used for efficient longest subsequence matching of the input bytes

#define HASH_TABLE_SIZE 256 // Hash function -> The middle byte will be used to find the index of the bytes
#define MAX_MATCHED_LENGTH 285



typedef struct hash_entry
{
    unsigned char str[3]; // Three consecutive bytes from the input stream hashed here
    int32_t loc; // Location of the entry within the sliding window of the input stream
    struct hash_entry *next;
} hash_entry;

typedef struct read_str
{
    unsigned char buf[290];
    int32_t count;
} read_str;

// Inline typedef of the struct hash_entry

int16_t hash_function(hash_entry);

bool cmp_hash_entry(hash_entry, hash_entry);

// Need to use 2d array for some specific reasons

hash_entry** init_hash_table();

int insert_table(hash_entry** , hash_entry);

int delete_chain(hash_entry* ); // Delete the hash entry of the chain after the provided hash_entry, yeah taking only pointer would work no need to pass the struct

hash_entry* search_table(hash_entry**, hash_entry); // don't think searching explicitly would be needed

// Read three upcoming bytes from the input stream and store it in the hash_entry and returns total bytes read from the stream 
int16_t get_next3_bytes(stream*,hash_entry*); 

void cleanup_hash(hash_entry**);

#endif
