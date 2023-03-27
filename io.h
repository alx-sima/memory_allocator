#ifndef __IO_H
#define __IO_H

/*
 * Coduri de eroare
 */
enum err_codes {
	INVALID_ADDRESS_FREE,
	INVALID_ALLOC_BLOCK,
	INVALID_COMMAND,
};

/*
 * Citeste de la tastatura o linie cu lungime necunoscuta
 * (alocand stringul pe heap).
 */
char *read_line(void);

/*
 * Parseaza stringul `s`, citind **exact** `nr` numere si
 * stocandu-le la adresele date ca parametru.
 * Returneaza 1 daca citirea s-a realizat cu succes.
 */
int read_numbers(char *s, int nr, ...);

/*
 * Printeaza mesajul de eroare corespunzator codului `err`.
 */
void print_err(enum err_codes err);

#endif //__IO_H
