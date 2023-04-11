/*
 * Operatii de input-output pentru blocuri de memorie.
 * Copyright: Sima Alexandru (312CA) 2023
 */
#ifndef __MEM_IO_H
#define __MEM_IO_H

#include "vma.h"

/*
 * Printeaza informatia blocului de la adresa `data`.
 * `args` trebuie sa pointeze la un long (indexul blocului).
 */
void print_block(void *data, void *args);

/*
 * Returneaza blocul care contine adresa `address` (daca exista).
 */
list_t *access_block(arena_t *arena, const uint64_t address);

/*
 * Returneaza miniblocul care contine adresa `address` (daca exista).
 */
list_t *access_miniblock(arena_t *arena, const uint64_t address);

/*
 * Returneaza miniblocul care incepe de la adresa `address` (daca exista).
 */
list_t *access_miniblock_start(arena_t *arena, const uint64_t address);

#endif // __MEM_IO_H
