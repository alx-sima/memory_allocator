#include <stdint.h>
#include <stdio.h>
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
	puts("Total memory: TODO");
	puts("Free memory: TODO");
	puts("Number of allocated blocks: TODO");
	puts("Number of allocated miniblocks: TODO");

	list_t *list_iter = arena->alloc_list;
	for (uint64_t block_no = 1; list_iter != NULL; ++block_no) {
		block_t *block = list_iter->data;
		puts("");
		printf("Block %lu begin\n", block_no);
		printf("Zone: %lu - %lu\n", block->start_address,
			   block->start_address + block->size);

		list_t *block_iter = block->miniblock_list;
		for (uint64_t miniblock_no = 1; block_iter != NULL; ++miniblock_no) {
			miniblock_t *miniblock = block_iter->data;
			printf("Miniblock %lu:\t\t%lu\t\t-\t\t%lu\t\t| %d\n", miniblock_no,
				   miniblock->start_address,
				   miniblock->start_address + miniblock->size, miniblock->perm);
			block_iter = block_iter->next;
		}

		printf("Block %lu end\n", block_no);
		list_iter = list_iter->next;
	}
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{
	(void)arena;
	(void)address;
	(void)permission;

	// TODO
}