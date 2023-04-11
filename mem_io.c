/*
 * Copyright: Sima Alexandru (312CA) 2023
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "mem_io.h"
#include "mem_prot.h"
#include "vma.h"

/*
 * Printeaza informatia miniblocului de la adresa `data`.
 * `args` trebuie sa pointeze la un long (indexul miniblocului).
 */
static void print_miniblock(void *data, void *args);

void print_block(void *data, void *args)
{
	block_t *block = data;
	uint64_t *block_nr = args;

	printf("\nBlock %lu begin\n", *block_nr);
	printf("Zone: 0x%lX - 0x%lX\n", block->start_address,
		   block->start_address + block->size);

	uint64_t miniblock_index = 1;
	apply_func(block->miniblock_list, print_miniblock, &miniblock_index);

	printf("Block %lu end\n", (*block_nr)++);
}

list_t *access_block(arena_t *arena, const uint64_t address)
{
	list_t *iter = arena->alloc_list;
	while (iter) {
		block_t *block = iter->data;

		if (address < block->start_address + block->size) {
			if (address >= block->start_address)
				return iter;
			return NULL;
		}

		iter = iter->next;
	}
	return NULL;
}

list_t *access_miniblock(arena_t *arena, const uint64_t address)
{
	list_t *block_list = access_block(arena, address);
	if (!block_list)
		return NULL;

	block_t *block = block_list->data;
	list_t *iter = block->miniblock_list;
	while (iter) {
		miniblock_t *miniblock = iter->data;

		if (address < miniblock->start_address + miniblock->size) {
			if (address >= miniblock->start_address)
				return iter;
			return NULL;
		}

		iter = iter->next;
	}
	return NULL;
}

list_t *access_miniblock_start(arena_t *arena, const uint64_t address)
{
	list_t *miniblock_list = access_miniblock(arena, address);
	if (!miniblock_list)
		return NULL;

	miniblock_t *miniblock = miniblock_list->data;
	if (miniblock->start_address == address)
		return miniblock_list;
	return NULL;
}

static void print_miniblock(void *data, void *args)
{
	miniblock_t *block = data;
	uint64_t *index = args;
	char perm[PERM_LEN + 1];
	get_perm_str(block->perm, perm);
	printf("Miniblock %lu:\t\t0x%lX\t\t-\t\t0x%lX\t\t| %s\n", (*index)++,
		   block->start_address, block->start_address + block->size, perm);
}
