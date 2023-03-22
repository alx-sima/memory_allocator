#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "vma.h"

int main(void)
{
	arena_t *arena = NULL;
	char *line;
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
		} else if (strcmp(command, "ALLOC_BLOCK") == 0) {
			// TODO
		} else if (strcmp(command, "FREE_BLOCK") == 0) {
			// TODO
		} else if (strcmp(command, "READ") == 0) {
			// TODO
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