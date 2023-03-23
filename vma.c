#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "io.h"
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

static block_t *init_block(const uint64_t address, const uint64_t size)
{
	block_t *block = malloc(sizeof(block_t));
	if (!block)
		return NULL;

	block->size = size;
	block->start_address = address;

	miniblock_t *miniblock = malloc(sizeof(miniblock_t));
	if (!miniblock) {
		free(block);
		return NULL;
	}

	miniblock->size = size;
	miniblock->start_address = address;

	list_t *list = malloc(sizeof(list_t));
	if (!list) {
		free(block);
		free(miniblock);
		return NULL;
	}

	list->data = miniblock;
	list->prev = NULL;
	list->next = NULL;

	block->miniblock_list = list;
	return block;
}

// !!!!
static inline int interval_overlap(int l1, int r1, int l2, int r2)
{
	return (l1 >= l2 && l2 < r1) || (l2 >= l1 && l1 < r2);
}

static int check_overlap(list_t *prev, const uint64_t address,
						 const uint64_t size)
{
	block_t *prev_block = prev->data;
	if (interval_overlap(address, address + size - 1, prev_block->start_address,
						 prev_block->start_address + prev_block->size - 1))
		return 1;

	list_t *next = prev->next;
	if (next) {
		block_t *next_block = next->data;
		if (interval_overlap(address, address + size, next_block->start_address,
							 next_block->start_address + next_block->size - 1))

			return 1;
	}

	return 0;
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	list_t *list_iter = arena->alloc_list;

	list_t *new = malloc(sizeof(list_t));
	if (!new) {
		// TODO
		return;
	}
	new->data = init_block(address, size);
	if (!new->data) {
		// TODO
		free(new);
		return;
	}

	if (!list_iter || address < ((block_t *)list_iter->data)->start_address) {
		new->prev = NULL;
		new->next = list_iter;
		arena->alloc_list = new;
		return;
	}

	while (list_iter->next) {
		block_t *curr_block = list_iter->data;
		if (curr_block->start_address < address)
			break;

		list_iter = list_iter->next;
	}

	if (check_overlap(list_iter, address, size)) {
		print_err(INVALID_ALLOC_BLOCK);
		free(new);
		return;
	}
	new->prev = list_iter;
	new->next = list_iter->next;

	if (new->next)
		((list_t *)list_iter->next)->prev = new;
	list_iter->next = new;
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