#include <alloca.h>
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
	char *line;
	// TODO: getline
	while ((line = read_line())) {
		char *command = strtok(line, " ");
		char *args = strtok(NULL, "");

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
			uint64_t address, block_size;
			if (*read_numbers(args, 2, &address, &block_size) == '\0')
				alloc_block(arena, address, block_size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "FREE_BLOCK") == 0) {
			uint64_t address;
			if (*read_numbers(args, 1, &address) == '\0')
				free_block(arena, address);
			else
				print_err(INVALID_COMMAND);

			// TODO
		} else if (strcmp(command, "READ") == 0) {
			uint64_t address, size;
			if (*read_numbers(args, 2, &address, &size) == '\0')
				read(arena, address, size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "WRITE") == 0) {
			uint64_t address, size;
			char *data_begin = read_numbers(args, 2, &address, &size) + 1;
			char *buffer = alloca(size);
			if (!buffer)
				return 1; // TODO
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

		free(line);
	}
	return 0;
}
