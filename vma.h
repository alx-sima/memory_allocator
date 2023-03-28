#ifndef __VMA_H
#define __VMA_H

#include <stddef.h>

#include "list.h"

typedef unsigned long long u64;
typedef unsigned char u8;

typedef struct {
	u64 start_address;
	size_t size;
	list_t *miniblock_list;
} block_t;

typedef struct {
	u64 start_address;
	size_t size;
	u8 perm;
	void *rw_buffer;
} miniblock_t;

typedef struct {
	u64 arena_size;
	list_t *alloc_list;
} arena_t;

arena_t *alloc_arena(const u64 size);
void dealloc_arena(arena_t *arena);

void alloc_block(arena_t *arena, const u64 address, const u64 size);
void free_block(arena_t *arena, const u64 address);

#endif // __VMA_H
