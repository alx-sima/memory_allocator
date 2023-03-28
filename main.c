#include <alloca.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "mem_io.h"
#include "vma.h"

int main(void)
{
	arena_t *arena = NULL;
	char *line = NULL;
	size_t line_size = 0;
	while (read_line(&line, &line_size)) {
		char *command = strtok(line, "\n ");
		char *args = strtok(NULL, "\n");

		if (strcmp(command, "ALLOC_ARENA") == 0) {
			uint64_t size;
			if (*read_numbers(args, 1, &size) == '\0')
				arena = alloc_arena(size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "DEALLOC_ARENA") == 0) {
			dealloc_arena(arena);
			free(line);
			exit(EXIT_SUCCESS); // TODO
		} else if (strcmp(command, "ALLOC_BLOCK") == 0) {
			uint64_t address, size;
			if (*read_numbers(args, 2, &address, &size) == '\0') {
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
		} else if (strcmp(command, "FREE_BLOCK") == 0) {
			uint64_t address;
			if (*read_numbers(args, 1, &address) == '\0')
				free_block(arena, address);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "READ") == 0) {
			uint64_t address, size;
			if (*read_numbers(args, 2, &address, &size) == '\0')
				read(arena, address, size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "WRITE") == 0) {
			uint64_t address, size;
			char *data_begin = read_numbers(args, 2, &address, &size) + 1;
			char *buffer = alloca(sizeof(char) * size);
			uint64_t bytes_read = strlen(data_begin) + 1;
			strncpy(buffer, data_begin, bytes_read);
			// TODO
			// fread(buffer + bytes_read, sizeof(char), size - bytes_read,
			// stdin);
			write(arena, address, size, (int8_t *)buffer);
		} else if (strcmp(command, "PMAP") == 0) {
			pmap(arena);
		} else {
			print_err(INVALID_COMMAND);
		}
	}
	free(line);
	return 0;
}
