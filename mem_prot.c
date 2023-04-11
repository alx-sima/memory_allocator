/*
 * Copyright: Sima Alexandru (312CA) 2023
 */
#include <string.h>

#include "io.h"
#include "mem_prot.h"

uint8_t parse_perm_str(char *perm_str)
{
	uint8_t perm = PROT_NONE;

	perm_str = strtok(perm_str, "\n| ");
	while (perm_str) {
		CHECK_PERM_STRING(perm_str, perm, PROT_READ);
		CHECK_PERM_STRING(perm_str, perm, PROT_WRITE);
		CHECK_PERM_STRING(perm_str, perm, PROT_EXEC);
		CHECK_PERM_STRING(perm_str, perm, PROT_NONE);
		perm_str = strtok(NULL, "\n| ");
	}

	return perm;
}

void get_perm_str(uint8_t perm, char perm_str[PERM_LEN + 1])
{
	// Se pleaca de la permisiuni complete, apoi se sterg cele
	// nesetate (`perm` & bit_permisiune == 0).
	memcpy(perm_str, "RWX", PERM_LEN + 1);
	for (uint8_t i = 0; i < PERM_LEN; ++i)
		if (~perm & (0b1 << i))
			perm_str[PERM_LEN - i - 1] = '-';
}

uint8_t check_perm(miniblock_t *miniblock, enum perm_bits perm)
{
	return miniblock->perm & perm;
}
