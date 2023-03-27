#ifndef __IO_H
#define __IO_H

/*
 * Coduri de eroare
 */
enum err_codes {
	INVALID_ALLOC_BLOCK,
	INVALID_ADDRESS_FREE,
	INVALID_ADDRESS_READ,
	INVALID_ADDRES_WRITE,
	INVALID_COMMAND,
};

/*
 * Citeste de la tastatura o linie cu lungime necunoscuta
 * (alocand stringul pe heap).
 */
char *read_line(void);

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

#endif //__IO_H
