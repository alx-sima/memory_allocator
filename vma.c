#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "list.h"
#include "vma.h"

void free_miniblock_data(void *data)
{
	free(data);
}

void free_block_data(void *data)
{
	block_t *block = data;
	list_t *list = block->miniblock_list;
	clear_list(list, free_miniblock_data);
	free(data);
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
	clear_list(arena->alloc_list, free_block_data);
	free(arena);
}

static miniblock_t *init_miniblock(const uint64_t address, const uint64_t size)
{
	miniblock_t *miniblock = malloc(sizeof(miniblock_t));
	if (!miniblock)
		return NULL;

	miniblock->size = size;
	miniblock->start_address = address;
	miniblock->perm = 0b110; // TODO permisiuni placeholder.
	return miniblock;
}

static block_t *init_block(const uint64_t address, const uint64_t size)
{
	block_t *block = malloc(sizeof(block_t));
	if (!block)
		return NULL;

	block->size = size;
	block->start_address = address;

	miniblock_t *miniblock = init_miniblock(address, size);
	if (!miniblock) {
		free(block);
		return NULL;
	}

	list_t *list = encapsulate(miniblock);
	if (!list) {
		free(block);
		free(miniblock);
		return NULL;
	}

	block->miniblock_list = list;
	return block;
}

inline static int check_overlap(block_t *prev, block_t *next)
{
	if (!prev || !next)
		return 0;

	// Blocurile se suprapun.
	return next->start_address < prev->start_address + prev->size;
}

block_t *merge_adjacent_blocks(block_t *prev, block_t *next)
{
	if (!prev)
		return next;
	if (!next)
		return prev;

	if (next->start_address == prev->start_address + prev->size) {
		merge_lists(prev->miniblock_list, next->miniblock_list);
		prev->size += next->size;
		free(next);
		return prev;
	}

	return next;
}

static list_t *get_prev_block(list_t *list, const uint64_t address)
{
	block_t *data = list->data;
	if (address < data->start_address)
		return NULL;
	return list;

	list_t *next;
	while ((next = list->next)) {
		data = next->data;
		if (address < data->start_address)
			return list;

		list = next;
	}

	return list;
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	block_t *new_block = init_block(address, size);
	if (!new_block)
		return;

	// TODO
	if (!arena->alloc_list) {
		list_t *new = encapsulate(new_block);
		if (!new) {
			free(new_block);
			return;
		}
		new->next = arena->alloc_list;
		arena->alloc_list = new;
		return;
	}

	list_t *const prev = get_prev_block(arena->alloc_list, address);
	list_t *const next = prev ? prev->next : arena->alloc_list;
	block_t *prev_block = prev ? prev->data : NULL;
	block_t *next_block = next ? next->data : NULL;

	if (check_overlap(prev_block, new_block) ||
		check_overlap(new_block, next_block)) {
		print_err(INVALID_ALLOC_BLOCK);
		free_block_data(new_block);
		return;
	}

	prev_block = merge_adjacent_blocks(prev_block, new_block);
	next_block = merge_adjacent_blocks(prev_block, next_block);

	if (prev_block == next_block) {
		remove_item(&arena->alloc_list, next);
		free(next);
	}
	if (prev_block == new_block) {
		list_t *new = encapsulate(new_block);
		// TODO
		if (!new) {}
		insert_after(&arena->alloc_list, prev, new);
	}
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
						return;
					}
					if (miniblock_iter->prev && miniblock_iter->next) {
						list_t *new_list_block = malloc(sizeof(list_t));
						block_t *new_block = malloc(sizeof(block_t));
						if (!new_block) {
							// TODO
							return;
						}
						new_list_block->data = new_block;
						new_block->miniblock_list = miniblock_iter->next;
						// TODO new_block->size
						insert_after(NULL, block_iter, new_list_block); // !
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
