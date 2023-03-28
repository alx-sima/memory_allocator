#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

size_t read_line(char **str, size_t *size)
{
	char buffer[BUFSIZ];

	size_t line_len = 0;
	while (fgets(buffer, BUFSIZ, stdin)) {
		size_t buffer_siz = strlen(buffer) + 1;

		if (line_len + buffer_siz > *size) {
			char *tmp = realloc(*str, sizeof(char) * (line_len + buffer_siz));
			if (!tmp)
				return 0;
			*size = line_len + buffer_siz;
			*str = tmp;
		}

		strncpy(*str + line_len, buffer, buffer_siz);
		line_len += buffer_siz - 1;
		if ((*str)[line_len - 1] == '\n')
			return line_len;
	}
	return 0;
}

char *read_numbers(char *str, int nr, ...)
{
	if (!str)
		return 0;

	va_list args;
	va_start(args, nr);

	char *next_char = str;
	for (int i = 0; i < nr; ++i) {
		uint64_t *adr = va_arg(args, uint64_t *);

		*adr = strtoll(str, &next_char, 0);
		if (next_char == str) {
			va_end(args);
			return 0;
		}
		str = next_char;
	}

	va_end(args);
	return next_char;
}

void print_err(enum err_codes err)
{
	switch (err) {
	case ADDRESS_OUT_OF_BOUNDS:
		puts("The allocated address is outside the size of arena");
		break;
	case END_OUT_OF_BOUNDS:
		puts("The end address is past the size of the arena");
		break;
	case INVALID_ALLOC_BLOCK:
		puts("This zone was already allocated.");
		break;
	case INVALID_ADDRESS_FREE:
		puts("Invalid address for free.");
		break;
	case INVALID_ADDRESS_READ:
		puts("Invalid address for read.");
		break;
	case INVALID_ADDRES_WRITE:
		puts("Invalid address for write.");
		break;
	case INVALID_COMMAND:
		puts("Invalid command. Please try again.");
		break;
	}
}
