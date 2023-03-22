#include <stdlib.h>

#include "vma.h"

arena_t *alloc_arena(const uint64_t size)
{
	arena_t *arena = malloc(sizeof(arena_t));
	if (!arena)
		return NULL;

	arena->arena_size = size;
	arena->alloc_list = NULL;
	return arena;
}

void dealloc_arena(arena_t *arena)
{
	if (!arena)
		return;

	// TODO

	free(arena);
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	(void)arena;
	(void)address;
	(void)size;

	// TODO
}

void free_block(arena_t *arena, const uint64_t address)
{
	(void)arena;
	(void)address;

	// TODO
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	(void)arena;
	(void)address;
	(void)size;

	// TODO
}

void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   int8_t *data)
{
	(void)arena;
	(void)address;
	(void)size;
	(void)data;

	// TODO
}

void pmap(const arena_t *arena)
{
	(void)arena;

	// TODO
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{
	(void)arena;
	(void)address;
	(void)permission;

	// TODO
}