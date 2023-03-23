#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

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

void remove_item(list_t **list, list_t *item)
{
	list_t *next = item->next;
	list_t *prev = item->prev;

	if (prev)
		prev->next = next;
	else
		*list = next;

	if (next)
		next->prev = prev;
}

void clear_list(list_t *list, void (*free_func)(void *))
{
	while (list) {
		list_t *next = list->next;
		free_func(list->data);
		free(list);
		list = next;
	}
}

void free_miniblock_func(void *ptr)
{
	free(ptr);
}

void free_block_func(void *ptr)
{
	block_t *block = ptr;
	list_t *list = block->miniblock_list;
	clear_list(list, free_miniblock_func);
	free(ptr);
}

void dealloc_arena(arena_t *arena)
{
	if (!arena)
		return;
	clear_list(arena->alloc_list, free_block_func);
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
	miniblock->perm = 0b110; // TODO permisiuni placeholder.

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

static int check_overlap(list_t *prev, const uint64_t address,
						 const uint64_t size)
{
	block_t *prev_data, *next_data;

	// Blocul se suprapune cu precedentul.
	if ((prev_data = prev->data) &&
		address < prev_data->start_address + prev_data->size)
		return 1;

	// Blocul se suprapune cu urmatorul.
	if (prev->next && (next_data = prev->next->data) &&
		address + size > next_data->start_address)
		return 1;

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

	list_t *next;
	while ((next = list_iter->next)) {
		block_t *next_block = next->data;
		if (address < next_block->start_address)
			break;

		list_iter = next;
	}

	if (check_overlap(list_iter, address, size)) {
		print_err(INVALID_ALLOC_BLOCK);
		// TODO
		free_block_func(new->data);
		free(new);
		return;
	}
	new->prev = list_iter;
	new->next = list_iter->next;

	if (new->next)
		list_iter->next->prev = new;
	list_iter->next = new;
}

void free_block(arena_t *arena, const uint64_t address)
{
	list_t *block_iter = arena->alloc_list;
	while (block_iter) {
		block_t *block = block_iter->data;
		if (address >= block->start_address) {
			if (address >= block->start_address + block->size) {
				print_err(INVALID_ADDRESS_FREE);
				return;
			}
			list_t *miniblock_iter = block->miniblock_list;
			while (miniblock_iter) {
				miniblock_t *mini = miniblock_iter->data;
				if (address == mini->start_address) {
					remove_item(&block->miniblock_list, miniblock_iter);

					// Blocul este gol: se sterge.
					if (!block->miniblock_list) {
						remove_item(&arena->alloc_list, block_iter);
					}
					return;
				}
				miniblock_iter = miniblock_iter->next;
			}

			return;
		}
		block_iter = block_iter->next;
	}

	print_err(INVALID_ADDRESS_FREE);
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

void print_list(list_t *list, void (*print_func)(uint64_t index, void *data))
{
	for (uint64_t i = 1; list != NULL; ++i) {
		print_func(i, list->data);
		list = list->next;
	}
}

void print_miniblock(uint64_t index, void *data)
{
	miniblock_t *block = data;
	printf("Miniblock %lu:\t\t%lu\t\t-\t\t%lu\t\t| %d\n", index,
		   block->start_address, block->start_address + block->size,
		   block->perm);
}

void print_block(uint64_t index, void *data)
{
	block_t *block = data;
	puts("");
	printf("Block %lu begin\n", index);
	printf("Zone: %lu - %lu\n", block->start_address,
		   block->start_address + block->size);
	print_list(block->miniblock_list, print_miniblock);
	printf("Block %lu end\n", index);
}

void pmap(const arena_t *arena)
{
	puts("Total memory: TODO");
	puts("Free memory: TODO");
	puts("Number of allocated blocks: TODO");
	puts("Number of allocated miniblocks: TODO");

	print_list(arena->alloc_list, print_block);
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{
	(void)arena;
	(void)address;
	(void)permission;

	// TODO
}