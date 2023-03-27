#ifndef __MEM_IO_H
#define __MEM_IO_H

#include "vma.h"

#define PERM_LEN 3

void read(arena_t *arena, uint64_t address, uint64_t size);
void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   int8_t *data);
void pmap(const arena_t *arena);
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);

#endif // __MEM_IO_H