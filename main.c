#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vma.h"

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

int main(void)
{
	char *line;
	while ((line = read_line())) {
		char *command, *args;
		command = strtok_r(line, " ", &args);
		if (strcmp(command, "ALLOC_ARENA") == 0) {
			// TODO
		} else if (strcmp(command, "DEALLOC_ARENA") == 0) {
			// TODO
		} else if (strcmp(command, "ALLOC_BLOCK") == 0) {
			// TODO
		} else if (strcmp(command, "FREE_BLOCK") == 0) {
			// TODO
		} else if (strcmp(command, "READ") == 0) {
			// TODO
		} else if (strcmp(command, "WRITE") == 0) {
			// TODO
		} else if (strcmp(command, "PMAP") == 0) {
			// TODO
		} else {
			puts("Invalid command. Please try again.\n");
		}

		free(line);
	}
	return 0;
}