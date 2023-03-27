#include <alloca.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "io.h"
#include "mem_io.h"
#include "vma.h"

static void get_perm(uint8_t perm, char perm_str[PERM_LEN + 1])
{
	for (uint8_t i = 0; i < PERM_LEN; ++i)
		if (~perm & (0b1 << i))
			perm_str[PERM_LEN - i - 1] = '-';
}

/*
 * Printeaza informatia miniblocului de la adresa `data`.
 * `args` trebuie sa pointeze la un long (indexul miniblocului).
 */
static void print_miniblock(void *data, void *args)
{
	miniblock_t *block = data;
	uint64_t *index = args;
	char perm[PERM_LEN + 1] = "RWX";
	get_perm(block->perm, perm);
	printf("Miniblock %lu:\t\t%lu\t\t-\t\t%lu\t\t| %s\n", (*index)++,
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

list_t *access_adress(arena_t *arena, const uint64_t address)
{
	list_t *block_iter = arena->alloc_list;
	while (block_iter) {
		block_t *block = block_iter->data;
		block_iter = block_iter->next;

		if (address < block->start_address)
			continue;

		list_t *miniblock_iter = block->miniblock_list;
		while (miniblock_iter) {
			miniblock_t *miniblock = miniblock_iter->data;

			if (address >= miniblock->start_address &&
				address < miniblock->start_address + miniblock->size)
				return miniblock_iter;

			miniblock_iter = miniblock_iter->next;
		}
		return NULL;
	}
	return NULL;
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	list_t *miniblock_list = access_adress(arena, address);
	if (!miniblock_list) {
		print_err(INVALID_ADDRESS_READ);
		return;
	}

	char *buffer = alloca(size);
	uint64_t bytes_read = 0;
	while (miniblock_list) {
		miniblock_t *miniblock = miniblock_list->data;
		if (bytes_read + miniblock->size > size) {
			memcpy(buffer + bytes_read, miniblock->rw_buffer,
				   size - bytes_read);
			fwrite(buffer, sizeof(char), bytes_read, stdout);
			puts("");
			return;
		}

		memcpy(buffer + bytes_read, miniblock->rw_buffer, miniblock->size);
		bytes_read += miniblock->size;
		miniblock_list = miniblock_list->next;
	}

	printf("Warning: size was bigger than the block size. Writing %lu "
		   "characters.\n",
		   bytes_read);
	fwrite(buffer, sizeof(char), bytes_read, stdout);
	puts("");
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
	printf("Total memory: 0x%lX\n", arena->arena_size);
	printf("Free memory: 0x%lX\n", free_memory);
	printf("Number of allocated blocks: %lu\n", no_blocks);
	printf("Number of allocated miniblocks: %lu\n", no_miniblocks);

	uint64_t block_index = 1;
	apply_func(arena->alloc_list, print_block, &block_index);
}
