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
			if (read_numbers(args, 1, &size))
				arena = alloc_arena(size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "DEALLOC_ARENA") == 0) {
			dealloc_arena(arena);
			free(line);
			exit(EXIT_SUCCESS); // TODO
		} else if (strcmp(command, "ALLOC_BLOCK") == 0) {
			uint64_t address, block_size;
			if (read_numbers(args, 2, &address, &block_size))
				alloc_block(arena, address, block_size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "FREE_BLOCK") == 0) {
			uint64_t address;
			if (read_numbers(args, 1, &address))
				free_block(arena, address);
			else
				print_err(INVALID_COMMAND);

			// TODO
		} else if (strcmp(command, "READ") == 0) {
			uint64_t address, size;
			if (read_numbers(args, 2, &address, &size))
				read(arena, address, size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "WRITE") == 0) {
			// TODO
		} else if (strcmp(command, "PMAP") == 0) {
			pmap(arena);
		} else {
			print_err(INVALID_COMMAND);
		}

		free(line);
	}

	return 0;
}
