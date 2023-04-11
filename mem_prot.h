/*
 * Operatii pentru verificarea permisiunilor
 * pe blocuri de memorie.
 * Copyright: Sima Alexandru (312CA) 2023
 */
#ifndef __MEM_PROT_H
#define __MEM_PROT_H

#include "vma.h"

/*
 * Lungimea stringului de permisiuni.
 */
#define PERM_LEN 3

/*
 * Bitii pentru permisiuni.
 */
enum perm_bits {
	PROT_READ = 0b100,
	PROT_WRITE = 0b010,
	PROT_EXEC = 0b001,
	PROT_NONE = 0b000,
};

/*
 * Daca `string` reprezinta permisiunea `perm_bit`,
 * se adauga permisiunea la `total_perm`.
 */
#define CHECK_PERM_STRING(string, total_perm, perm_bit)                        \
	do {                                                                       \
		if (strcmp(string, #perm_bit) == 0)                                    \
			(total_perm) |= perm_bit;                                          \
	} while (0)

/*
 * Calculeaza reprezentarea pe biti
 * a permisiunilor din `perm_str`.
 */
uint8_t parse_perm_str(char *perm_str);

/*
 * Scrie in `perm_str` reprezentarea in
 * format text a permisiunilor din `perm`.
 */
void get_perm_str(uint8_t perm, char perm_str[PERM_LEN + 1]);

/*
 * Verifica daca `miniblock` are permisiunea `perm`.
 */
uint8_t check_perm(miniblock_t *miniblock, enum perm_bits perm);

#endif // __MEM_PROT_H
