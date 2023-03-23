#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

char *read_line()
{
	char buffer[BUFSIZ];

	// Initial se aloca un string "gol" (care contine doar un '\0').
	char *line = calloc(1, sizeof(char));
	size_t line_length = 1;

	while (fgets(buffer, BUFSIZ, stdin)) {
		size_t buffer_len = strlen(buffer);
		line_length += buffer_len;

		char *temp = realloc(line, line_length);
		if (!temp) {
			free(line);
			return NULL;
		}
		line = temp;
		// Se concateneaza caracterele citite in aceasta transa la rezultat.
		strncat(line, buffer, buffer_len);

		// Daca ultimul caracter citit este newline,
		// s-a terminat de citit linia.
		if (line[line_length - 2] == '\n') {
			// Se elimina newline-ul.
			line[line_length - 2] = '\0';
			return line;
		}
	}

	// In cazul in care nu s-a putut citi linia,
	// se elibereaza stringul si se intoarce NULL.
	free(line);
	return NULL;
}

int read_numbers(char *s, int nr, ...)
{
	if (!s)
		return 0;

	va_list args;
	va_start(args, nr);

	char *endptr = s;
	for (int i = 0; i < nr; ++i) {
		uint64_t *adr = va_arg(args, uint64_t *);

		*adr = strtoll(s, &endptr, 0);
		if (endptr == s) {
			va_end(args);
			return 0;
		}
		s = endptr;
	}

	va_end(args);
	// Nu mai exista caractere de citit.
	return *endptr == '\0';
}

void print_err(enum err_codes err)
{
	switch (err) {
	case INVALID_ALLOC_BLOCK:
		puts("This zone was already allocated.");
		break;
	case INVALID_COMMAND:
		puts("Invalid command. Please try again.");
		break;
	}
}
