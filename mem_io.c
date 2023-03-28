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
	u64 *index = args;
	char perm[PERM_LEN + 1];
	get_perm_str(block->perm, perm);
	printf("Miniblock %llu:\t\t0x%llX\t\t-\t\t0x%llX\t\t| %s\n", (*index)++,
		   block->start_address, block->start_address + block->size, perm);
}

/*
 * Printeaza informatia blocului de la adresa `data`.
 * `args` trebuie sa pointeze la 1 long (indexul blocului).
 */
static void print_block(void *data, void *args)
{
	block_t *block = data;
	u64 *block_nr = args;

	printf("\nBlock %llu begin\n", *block_nr);
	printf("Zone: 0x%llX - 0x%llX\n", block->start_address,
		   block->start_address + block->size);

	u64 miniblock_index = 1;
	apply_func(block->miniblock_list, print_miniblock, &miniblock_index);

	printf("Block %llu end\n", (*block_nr)++);
}

list_t *access_block(arena_t *arena, const u64 address)
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

list_t *access_miniblock(arena_t *arena, const u64 address)
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

list_t *access_miniblock_start(arena_t *arena, const u64 address)
{
	list_t *miniblock_list = access_miniblock(arena, address);
	if (!miniblock_list)
		return NULL;

	miniblock_t *miniblock = miniblock_list->data;
	if (miniblock->start_address == address)
		return miniblock_list;
	return NULL;
}

static inline u64 min(u64 a, u64 b)
{
	return a < b ? a : b;
}

void read(arena_t *arena, u64 address, u64 size)
{
	list_t *miniblock_iter = access_miniblock(arena, address);
	if (!miniblock_iter) {
		print_err(INVALID_ADDRESS_READ);
		return;
	}

	char *buffer = malloc(sizeof(char) * size);
	// TODO if (!buffer) return;
	u64 bytes_read = 0;
	miniblock_t *miniblock = miniblock_iter->data;
	miniblock_iter = miniblock_iter->next;
	u64 offset = address - miniblock->start_address;
	u64 batch = min(size - bytes_read, miniblock->size - offset);
	memcpy(buffer, miniblock->rw_buffer + offset, batch);
	bytes_read += batch;

	while (miniblock_iter && size != bytes_read) {
		miniblock = miniblock_iter->data;
		miniblock_iter = miniblock_iter->next;

		if (!check_perm(miniblock, PROT_READ)) {
			print_err(INVALID_PERMISSIONS_READ);
			free(buffer);
			return;
		}
		u8 batch = min(size - bytes_read, miniblock->size);
		memcpy(buffer + bytes_read, miniblock->rw_buffer, batch);
		bytes_read += batch;
	}
	if (size != bytes_read)
		printf("Warning: size was bigger than the block size. Reading %llu "
			   "characters.\n",
			   bytes_read);
	fwrite(buffer, sizeof(char), bytes_read, stdout);
	free(buffer);
	puts("");
}

void write(arena_t *arena, const u64 address, const u64 size, u8 *data)
{
	(void)arena;
	(void)address;
	(void)size;
	(void)data;

	list_t *miniblock_iter = access_miniblock(arena, address);
	if (!miniblock_iter) {
		print_err(INVALID_ADDRESS_WRITE);
		return;
	}

	miniblock_t *miniblock = miniblock_iter->data;
	u64 bytes_written = 0;
	void *write_addr =
		miniblock->rw_buffer + address - miniblock->start_address;
	for (;;) {
		if (!check_perm(miniblock, PROT_WRITE)) {
			// TODO
			print_err(INVALID_PERMISSIONS_WRITE);
			return;
		}
		if (bytes_written + miniblock->size >= size) {
			memcpy(write_addr, data + bytes_written, size - bytes_written);
			return;
		}
		memcpy(write_addr, data + bytes_written, miniblock->size);
		bytes_written += miniblock->size;

		miniblock_iter = miniblock_iter->next;
		if (!miniblock_iter)
			break;
		miniblock = miniblock_iter->data;
		write_addr = miniblock->rw_buffer;
	}
	printf("Warning: size was bigger than the block size. Writing %llu "
		   "characters.\n",
		   bytes_written);
}

void pmap(const arena_t *arena)
{
	// TODO refactor
	u64 no_blocks = 0;
	u64 no_miniblocks = 0;
	u64 free_memory = arena->arena_size;
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
	printf("Total memory: 0x%llX bytes\n", arena->arena_size);
	printf("Free memory: 0x%llX bytes\n", free_memory);
	printf("Number of allocated blocks: %llu\n", no_blocks);
	printf("Number of allocated miniblocks: %llu\n", no_miniblocks);

	u64 block_index = 1;
	apply_func(arena->alloc_list, print_block, &block_index);
}
