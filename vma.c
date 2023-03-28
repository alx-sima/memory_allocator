#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "list.h"
#include "mem_io.h"
#include "vma.h"

void free_miniblock_data(void *data)
{
	miniblock_t *miniblock = data;
	free(miniblock->rw_buffer);
	free(data);
}

void free_block_data(void *data)
{
	block_t *block = data;
	list_t *list = block->miniblock_list;
	clear_list(list, free_miniblock_data);
	free(data);
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
	miniblock->perm = 0b110;			 // TODO permisiuni placeholder.
	miniblock->rw_buffer = malloc(size); // TODO clarificare cerinta
	if (!miniblock->rw_buffer) {
		free(miniblock);
		return NULL;
	}
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

/*
 * Verifica daca blocul `next` incepe inainte sa se termine blocul `prev`.
 */
static inline int check_overlap(block_t *prev, block_t *next)
{
	if (!prev || !next)
		return 0;

	return next->start_address < prev->start_address + prev->size;
}

/*
 * Daca blocurile `prev` si `next` sunt adiacente, se
 * muta miniblocurile in `prev` si se elibereaza `next`.
 *
 * Returneaza blocul in care se afla nodurile din `next` dupa operatie.
 */
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

/*
 * Returneaza ultimul nod cu adresa de start
 * mai mica decat `address` (daca exista).
 */
static list_t *get_prev_block(list_t *list, const uint64_t address)
{
	block_t *data = list->data;
	if (address < data->start_address)
		return NULL;

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

	// Blocul s-a unit cu urmatorul,
	// se elibereaza nodul urmator.
	if (prev_block == next_block) {
		if (next) {
			remove_item(&arena->alloc_list, next);
			free(next);
		}
	}
	// Blocul nu s-a unit cu anteriorul,
	// se aloca un bloc nou dupa acesta.
	if (prev_block == new_block) {
		list_t *new = encapsulate(new_block);
		if (!new)
			return; // TODO
		insert_after(&arena->alloc_list, prev, new);
	}
}

void free_block(arena_t *arena, const uint64_t address)
{
	list_t *block_iter = access_block(arena, address);
	if (!block_iter) {
		print_err(INVALID_ADDRESS_FREE);
		return;
	}

	block_t *block = block_iter->data;
	list_t *miniblock_iter = block->miniblock_list;
	uint64_t prev_miniblocks_size = 0;
	while (miniblock_iter) {
		miniblock_t *mini = miniblock_iter->data;
		if (address != mini->start_address) {
			miniblock_iter = miniblock_iter->next;
			prev_miniblocks_size += mini->size;
			continue;
		}
		remove_item(&block->miniblock_list, miniblock_iter);

		// Blocul este gol: se sterge.
		if (!block->miniblock_list) {
			remove_item(&arena->alloc_list, block_iter);
			// TODO
			free_miniblock_data(mini);
			free(miniblock_iter);
			free_block_data(block);
			free(block_iter);
			return;
		}
		if (miniblock_iter->prev && miniblock_iter->next) {
			block_t *new_block = malloc(sizeof(block_t));
			if (!new_block) {
				// TODO
				return;
			}
			new_block->miniblock_list = miniblock_iter->next;
			new_block->start_address =
				((miniblock_t *)miniblock_iter->next->data)->start_address;
			new_block->size = block->size - new_block->start_address;
			block->size = prev_miniblocks_size;
			miniblock_iter->next->prev = NULL;
			miniblock_iter->prev->next = NULL;
			// TODO new_block->size
			insert_after(NULL, block_iter,
						 encapsulate(new_block)); // !

		} else {
			if (!miniblock_iter->prev)
				block->start_address = mini->start_address;
			block->size -= mini->size;
		}
		free_miniblock_data(mini);
		free(miniblock_iter);
		return;
	}

	print_err(INVALID_ADDRESS_FREE);
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{
	(void)arena;
	(void)address;
	(void)permission;

	// TODO
}
