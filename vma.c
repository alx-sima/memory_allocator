/*
 * Copyright: Sima Alexandru (312CA) 2023
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "list.h"
#include "mem_alloc.h"
#include "mem_io.h"
#include "mem_prot.h"
#include "utils.h"
#include "vma.h"

/*
 * Verifica daca blocul `next` incepe inainte sa se termine blocul `prev`.
 */
static inline int check_overlap(block_t *prev, block_t *next);

/*
 * Daca blocurile `prev` si `next` sunt adiacente, se
 * muta miniblocurile in `prev` si se elibereaza `next`.
 *
 * Returneaza blocul in care se afla nodurile din `next` dupa operatie.
 */
static block_t *merge_adjacent_blocks(block_t *prev, block_t *next);

/*
 * Returneaza ultimul nod cu adresa de start
 * mai mica decat `address` (daca exista).
 */
static list_t *get_prev_block(list_t *list, const uint64_t address);

arena_t *alloc_arena(const uint64_t size)
{
	arena_t *arena = malloc(sizeof(arena_t));
	if (!arena)
		return NULL;

	arena->arena_size = size;
	arena->alloc_list = NULL;
	arena->has_error = 0;
	return arena;
}

void dealloc_arena(arena_t *arena)
{
	if (!arena)
		return;
	clear_list(arena->alloc_list, free_block_data);
	free(arena);
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	block_t *new_block = init_block(address, size);
	if (!new_block)
		return;

	if (!arena->alloc_list) {
		list_t *new = encapsulate(new_block);
		if (!new) {
			arena->has_error = 1;
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

	// Blocul se suprapune cu un altul.
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
		if (!new) {
			arena->has_error = 1;
			return;
		}
		insert_after(&arena->alloc_list, prev, new);
	}
}

void free_block(arena_t *arena, const uint64_t address)
{
	list_t *block_list = access_block(arena, address);
	if (!block_list) {
		print_err(INVALID_ADDRESS_FREE);
		return;
	}

	block_t *block = block_list->data;
	list_t *iter = block->miniblock_list;
	while (iter) {
		miniblock_t *miniblock = iter->data;
		if (address != miniblock->start_address) {
			iter = iter->next;
			continue;
		}

		remove_item(&block->miniblock_list, iter);

		// Blocul este gol: se sterge.
		if (!block->miniblock_list) {
			remove_item(&arena->alloc_list, block_list);
			free_miniblock_data(miniblock);
			free_block_data(block);
			free(block_list);
			free(iter);
			return;
		}
		// Miniblocul are vecini in ambele parti, asa ca
		// miniblocurile incepand cu urmatorul sunt mutate in alt bloc.
		if (iter->prev && iter->next) {
			block_t *new_block = malloc(sizeof(block_t));
			if (!new_block) {
				arena->has_error = 1;
				return;
			}

			miniblock_t *next = iter->next->data;
			uint64_t block_end_address = block->start_address + block->size;

			// Vechiul bloc se va termina inaintea miniblocului sters,
			// asa ca lungimea lui va fi distanta adreselor lor.
			block->size = miniblock->start_address - block->start_address;

			new_block->miniblock_list = iter->next;
			new_block->start_address = next->start_address;
			new_block->size = block_end_address - new_block->start_address;

			iter->next->prev = NULL;
			iter->prev->next = NULL;
			list_t *new_node = encapsulate(new_block);
			if (!new_block) {
				free(new_block);
				arena->has_error = 1;
				return;
			}

			insert_after(&arena->alloc_list, block_list, new_node);
		} else {
			// Nodul este la marginea unui bloc, asa ca doar se scade
			// dimensiunea acestuia si, eventual, se modifica capul listei de
			// noduri.
			if (!iter->prev) {
				miniblock_t *next = iter->next->data;
				block->start_address = next->start_address;
			}
			block->size -= miniblock->size;
		}
		free_miniblock_data(miniblock);
		free(iter);
		return;
	}

	print_err(INVALID_ADDRESS_FREE);
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	list_t *iter = access_miniblock(arena, address);
	if (!iter) {
		print_err(INVALID_ADDRESS_READ);
		return;
	}

	char *buffer = malloc(sizeof(char) * size);
	if (!buffer) {
		arena->has_error = 1;
		return;
	}

	miniblock_t *miniblock = iter->data;
	iter = iter->next;
	if (!check_perm(miniblock, PROT_READ)) {
		print_err(INVALID_PERMISSIONS_READ);
		free(buffer);
		return;
	}
	uint64_t offset = address - miniblock->start_address;
	uint64_t batch = min(size, miniblock->size - offset);
	uint64_t bytes_read = batch;
	memcpy(buffer, miniblock->rw_buffer + offset, batch);

	while (iter && bytes_read != size) {
		miniblock = iter->data;
		iter = iter->next;

		if (!check_perm(miniblock, PROT_READ)) {
			print_err(INVALID_PERMISSIONS_READ);
			free(buffer);
			return;
		}

		batch = min(size - bytes_read, miniblock->size);
		memcpy(buffer + bytes_read, miniblock->rw_buffer, batch);
		bytes_read += batch;
	}

	if (bytes_read != size)
		printf("Warning: size was bigger than the block size. Reading %lu "
			   "characters.\n",
			   bytes_read);

	uint64_t out_len = max_len(buffer, bytes_read);
	fwrite(buffer, sizeof(char), out_len, stdout);
	free(buffer);
	puts("");
}

void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   char *data)
{
	list_t *iter = access_miniblock(arena, address);
	if (!iter) {
		print_err(INVALID_ADDRESS_WRITE);
		return;
	}

	miniblock_t *miniblock = iter->data;
	iter = iter->next;
	if (!check_perm(miniblock, PROT_WRITE)) {
		print_err(INVALID_PERMISSIONS_WRITE);
		return;
	}

	uint64_t offset = address - miniblock->start_address;
	uint64_t batch = min(size, miniblock->size - offset);
	uint64_t bytes_written = batch;
	memcpy(miniblock->rw_buffer + offset, data, batch);

	while (iter && bytes_written != size) {
		if (!check_perm(miniblock, PROT_WRITE)) {
			print_err(INVALID_PERMISSIONS_WRITE);
			return;
		}
		miniblock = iter->data;
		iter = iter->next;

		batch = min(size - bytes_written, miniblock->size);
		memcpy(miniblock->rw_buffer, data + bytes_written, batch);
		bytes_written += batch;
	}
	if (bytes_written != size)
		printf("Warning: size was bigger than the block size. Writing %lu "
			   "characters.\n",
			   bytes_written);
}

void pmap(const arena_t *arena)
{
	uint64_t no_blocks = 0;
	uint64_t no_miniblocks = 0;
	uint64_t free_memory = arena->arena_size;

	list_t *block_iter = arena->alloc_list;
	while (block_iter) {
		block_t *block = block_iter->data;
		free_memory -= block->size;

		list_t *miniblock_iter = block->miniblock_list;
		while (miniblock_iter) {
			++no_miniblocks;
			miniblock_iter = miniblock_iter->next;
		}

		++no_blocks;
		block_iter = block_iter->next;
	}

	printf("Total memory: 0x%lX bytes\n", arena->arena_size);
	printf("Free memory: 0x%lX bytes\n", free_memory);
	printf("Number of allocated blocks: %lu\n", no_blocks);
	printf("Number of allocated miniblocks: %lu\n", no_miniblocks);

	uint64_t block_index = 1;
	apply_func(arena->alloc_list, print_block, &block_index);
}

void mprotect(arena_t *arena, uint64_t address, char *permission)
{
	uint8_t perm = parse_perm_str(permission);
	list_t *miniblock_node = access_miniblock_start(arena, address);
	if (!miniblock_node) {
		print_err(INVALID_ADDRESS_MPROTECT);
		return;
	}

	miniblock_t *miniblock = miniblock_node->data;
	miniblock->perm = perm;
}

static inline int check_overlap(block_t *prev, block_t *next)
{
	if (!prev || !next)
		return 0;

	return next->start_address < prev->start_address + prev->size;
}

static block_t *merge_adjacent_blocks(block_t *prev, block_t *next)
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

	list_t *next;
	while ((next = list->next)) {
		data = next->data;
		if (address < data->start_address)
			return list;

		list = next;
	}

	return list;
}
