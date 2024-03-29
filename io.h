/*
 * Operatii de input-output ale programului.
 * Copyright: Sima Alexandru (312CA) 2023
 */
#ifndef __IO_H
#define __IO_H

#include <stddef.h>

#include "vma.h"

/*
 * Coduri de eroare
 */
enum err_codes {
	ADDRESS_OUT_OF_BOUNDS,
	END_OUT_OF_BOUNDS,
	INVALID_ALLOC_BLOCK,
	INVALID_ADDRESS_FREE,
	INVALID_ADDRESS_READ,
	INVALID_ADDRESS_WRITE,
	INVALID_ADDRESS_MPROTECT,
	INVALID_PERMISSIONS_READ,
	INVALID_PERMISSIONS_WRITE,
	INVALID_COMMAND,
};

/*
 * Citeste de la tastatura o linie cu lungime necunoscuta, retinand rezultatul
 * in `*str` de lungime `*size`, pe care il (re)aloca dinamic, daca este
 * necesar. Returneaza nr. de octeti cititi.
 */
size_t read_line(char **str, size_t *size);

/*
 * Parseaza stringul `str`, citind `nr` numere si
 * stocandu-le la adresele date ca parametru.
 * Returneaza adresa urmatorului caracter.
 */
char *read_numbers(char *str, int nr, ...);

/*
 * Printeaza mesajul de eroare corespunzator codului `err`.
 */
void print_err(enum err_codes err);

void parse_alloc_arena_command(arena_t **arena, char *args);

void parse_alloc_block_command(arena_t *arena, char *args);

void parse_free_command(arena_t *arena, char *args);

void parse_read_command(arena_t *arena, char *args);

void parse_write_command(arena_t *arena, char *args, char **read_buffer,
						 size_t *buffer_size);

void parse_mprotect_command(arena_t *arena, char *args);

#endif //__IO_H
