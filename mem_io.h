/*
 * Operatii de input-output pentru blocuri de memorie.
 */
#ifndef __MEM_IO_H
#define __MEM_IO_H

#include "vma.h"

uint64_t min(uint64_t a, uint64_t b);

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

/*
 * Citeste si afiseaza `size` octeti de la adresa `address`.
 * Afiseaza mesaje de eroare daca nu e posibil.
 */
void read(arena_t *arena, uint64_t address, uint64_t size);

/*
 * Scrie `size` octeti din `data` la adresa `address`.
 * Afiseaza mesaje de eroare daca nu e posibil.
 */
void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   uint8_t *data);

/*
 * Printeaza harta de memorie a `arenei`.
 */
void pmap(const arena_t *arena);

#endif // __MEM_IO_H
