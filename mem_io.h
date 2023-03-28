/*
 * Operatii de input-output pentru blocuri de memorie.
 */
#ifndef __MEM_IO_H
#define __MEM_IO_H

#include "vma.h"

uint64_t min(uint64_t a, uint64_t b);
list_t *access_block(arena_t *arena, const uint64_t address);
list_t *access_miniblock(arena_t *arena, const uint64_t address);
list_t *access_miniblock_start(arena_t *arena, const uint64_t address);

void read(arena_t *arena, uint64_t address, uint64_t size);
void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   uint8_t *data);
void pmap(const arena_t *arena);

#endif // __MEM_IO_H
