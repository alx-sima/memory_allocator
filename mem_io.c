#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "list.h"
#include "mem_io.h"
#include "mem_prot.h"
#include "vma.h"

/*
 * Printeaza informatia miniblocului de la adresa `data`.
 * `args` trebuie sa pointeze la un long (indexul miniblocului).
 */
static void print_miniblock(void *data, void *args)
{
	miniblock_t *block = data;
	uint64_t *index = args;
	char perm[PERM_LEN + 1];
	get_perm_str(block->perm, perm);
	printf("Miniblock %lu:\t\t0x%lX\t\t-\t\t0x%lX\t\t| %s\n", (*index)++,
		   block->start_address, block->start_address + block->size, perm);
}

/*
 * Printeaza informatia blocului de la adresa `data`.
 * `args` trebuie sa pointeze la 1 long (indexul blocului).
 */
static void print_block(void *data, void *args)
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

uint64_t min(uint64_t a, uint64_t b)
{
	return a < b ? a : b;
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	list_t *iter = access_miniblock(arena, address);
	if (!iter) {
		print_err(INVALID_ADDRESS_READ);
		return;
	}

	char *buffer = malloc(sizeof(char) * size);
	// TODO if (!buffer) return;

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
	uint64_t out_len = min(bytes_read, strlen(buffer));
	fwrite(buffer, sizeof(char), out_len, stdout);
	free(buffer);
	puts("");
}

void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   uint8_t *data)
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
	// TODO refactor
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
