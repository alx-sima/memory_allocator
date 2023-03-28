#ifndef __MEM_IO_H
#define __MEM_IO_H

#include "vma.h"

#define PERM_LEN 3

list_t *access_block(arena_t *arena, const u64 address);
list_t *access_miniblock(arena_t *arena, const u64 address);

void read(arena_t *arena, u64 address, u64 size);
void write(arena_t *arena, const u64 address, const u64 size, u8 *data);
void pmap(const arena_t *arena);
void mprotect(arena_t *arena, u64 address, u8 permission);

#endif // __MEM_IO_H
