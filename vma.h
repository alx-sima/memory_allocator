/*
 * API-ul alocatorului de memorie.
 * Copyright: Sima Alexandru (312CA) 2023
 */
#ifndef __VMA_H
#define __VMA_H

#include <stddef.h>
#include <stdint.h>

#include "list.h"

typedef struct {
	uint64_t start_address;
	size_t size;
	list_t *miniblock_list;
} block_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	uint8_t perm;
	void *rw_buffer;
} miniblock_t;

typedef struct {
	uint64_t arena_size;
	list_t *alloc_list;
	int has_error;
} arena_t;

arena_t *alloc_arena(const uint64_t size);
void dealloc_arena(arena_t *arena);

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);
void free_block(arena_t *arena, const uint64_t address);

void read(arena_t *arena, uint64_t address, uint64_t size);
void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   char *data);

void pmap(const arena_t *arena);
void mprotect(arena_t *arena, uint64_t address, char *permission);

#endif // __VMA_H
