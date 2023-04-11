/*
 * Copyright: Sima Alexandru (312CA) 2023
 */
#include <stdlib.h>

#include "mem_alloc.h"
#include "mem_prot.h"

/*
 * Initializeaza un minibloc nou la adresa `address` si de dimensiune `size`.
 */
static miniblock_t *init_miniblock(const uint64_t address, const uint64_t size);

block_t *init_block(const uint64_t address, const uint64_t size)
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

static miniblock_t *init_miniblock(const uint64_t address, const uint64_t size)
{
	miniblock_t *miniblock = malloc(sizeof(miniblock_t));
	if (!miniblock)
		return NULL;

	miniblock->size = size;
	miniblock->start_address = address;
	// Seteaza permisiunile default.
	miniblock->perm = PROT_READ | PROT_WRITE;
	miniblock->rw_buffer = calloc(size, sizeof(char));
	if (!miniblock->rw_buffer) {
		free(miniblock);
		return NULL;
	}
	return miniblock;
}
