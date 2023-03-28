#ifndef __MEM_IO_H
#define __MEM_IO_H

#include "vma.h"

#define PERM_LEN 3

enum perm_bits {
	PROT_READ = 0b100,
	PROT_WRITE = 0b010,
	PROT_EXEC = 0b001,
	PROT_NONE = 0b000,
};

#define CHECK_PERM_STRING(string, total_perm, perm_bit)                        \
	do {                                                                       \
		if (strcmp(string, #perm_bit) == 0)                                    \
			(total_perm) |= perm_bit;                                          \
	} while (0)

list_t *access_block(arena_t *arena, const u64 address);
list_t *access_miniblock(arena_t *arena, const u64 address);

void read(arena_t *arena, u64 address, u64 size);
void write(arena_t *arena, const u64 address, const u64 size, u8 *data);
void pmap(const arena_t *arena);
void mprotect(arena_t *arena, u64 address, u8 permission);

#endif // __MEM_IO_H
