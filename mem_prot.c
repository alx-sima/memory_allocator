#include <string.h>

#include "io.h"
#include "mem_io.h"
#include "mem_prot.h"

void get_perm_str(u8 perm, char perm_str[PERM_LEN + 1])
{
	memcpy(perm_str, "RWX", PERM_LEN + 1);
	for (u8 i = 0; i < PERM_LEN; ++i)
		if (~perm & (0b1 << i))
			perm_str[PERM_LEN - i - 1] = '-';
}

int check_perm(miniblock_t *miniblock, enum perm_bits perm)
{
	return miniblock->perm & perm;
}

void mprotect(arena_t *arena, u64 address, u8 permission)
{
	list_t *miniblock_node = access_miniblock(arena, address);
	if (!miniblock_node) {
		print_err(INVALID_ADDRESS_MPROTECT);
		return;
	}

	miniblock_t *miniblock = miniblock_node->data;
	miniblock->perm = permission;
}
