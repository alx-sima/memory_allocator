#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "mem_io.h"
#include "mem_prot.h"
#include "vma.h"

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
		return NULL;

	va_list args;
	va_start(args, nr);

	char *next_char = str;
	for (int i = 0; i < nr; ++i) {
		uint64_t *adr = va_arg(args, uint64_t *);

		*adr = strtoll(str, &next_char, 0);
		if (next_char == str) {
			va_end(args);
			return next_char;
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
	case INVALID_ADDRESS_WRITE:
		puts("Invalid address for write.");
		break;
	case INVALID_ADDRESS_MPROTECT:
		puts("Invalid address for mprotect.");
		break;
	case INVALID_PERMISSIONS_READ:
		puts("Invalid permissions for read.");
		break;
	case INVALID_PERMISSIONS_WRITE:
		puts("Invalid permissions for write.");
		break;
	case INVALID_COMMAND:
		puts("Invalid command. Please try again.");
		break;
	}
}

int parse_alloc_arena_command(arena_t **arena, char *args)
{
	uint64_t size;
	if (*read_numbers(args, 1, &size) == '\n')
		return !(*arena = alloc_arena(size));
	print_err(INVALID_COMMAND);
	return 0;
}

int parse_alloc_block_command(arena_t *arena, char *args)
{
	uint64_t address, size;
	if (*read_numbers(args, 2, &address, &size) == '\n') {
		if (address >= arena->arena_size) {
			print_err(ADDRESS_OUT_OF_BOUNDS);
		} else {
			if (address + size <= arena->arena_size)
				alloc_block(arena, address, size);
			else
				print_err(END_OUT_OF_BOUNDS);
		}
	} else {
		print_err(INVALID_COMMAND);
	}
	return 0;
}

void parse_free_command(arena_t *arena, char *args)
{
	uint64_t address;
	if (*read_numbers(args, 1, &address) == '\n')
		free_block(arena, address);
	else
		print_err(INVALID_COMMAND);
}

int parse_read_command(arena_t *arena, char *args)
{
	uint64_t address, size;
	if (*read_numbers(args, 2, &address, &size) == '\n')
		read(arena, address, size);
	else
		print_err(INVALID_COMMAND);

	return 0;
}

int parse_write_command(arena_t *arena, char *args, char **read_buffer,
						size_t *buffer_size)
{
	uint64_t address, size;
	char *data_begin = read_numbers(args, 2, &address, &size);
	if (*data_begin == ' ')
		++data_begin;
	uint8_t *buffer = malloc(sizeof(uint8_t) * size);
	if (!buffer)
		return 1;

	uint64_t batch = min(size, strlen(data_begin));
	memcpy(buffer, data_begin, batch);
	uint64_t bytes_read = batch;
	while (bytes_read < size) {
		size_t line_length = read_line(read_buffer, buffer_size);
		batch = min(size - bytes_read, line_length);
		memcpy(buffer + bytes_read, *read_buffer, batch);
		bytes_read += batch;
	}
	write(arena, address, size, buffer);
	if ((*read_buffer)[batch] != '\n') {
		char *tmp = strtok(data_begin + batch, "\n ");
		while (tmp) {
			print_err(INVALID_COMMAND);
			tmp = strtok(NULL, "\n ");
		}
	}
	free(buffer);

	return 0;
}

void parse_mprotect_command(arena_t *arena, char *args)
{
	uint64_t address;
	char *perm_str = read_numbers(args, 1, &address);
	mprotect(arena, address, (int8_t *)perm_str);
}
