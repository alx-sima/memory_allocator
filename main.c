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
		int err = 0;
		char *command = strtok(line, "\n ");
		if (!command)
			continue;

		char *args = strtok(NULL, "");
		if (strcmp(command, "ALLOC_ARENA") == 0) {
			err = parse_alloc_arena_command(&arena, args);
		} else if (strcmp(command, "DEALLOC_ARENA") == 0) {
			dealloc_arena(arena);
			break;
		} else if (strcmp(command, "ALLOC_BLOCK") == 0) {
			err = parse_alloc_block_command(arena, args);
		} else if (strcmp(command, "FREE_BLOCK") == 0) {
			parse_free_command(arena, args);
		} else if (strcmp(command, "READ") == 0) {
			err = parse_read_command(arena, args);
		} else if (strcmp(command, "WRITE") == 0) {
			err = parse_write_command(arena, args, &line, &line_size);
		} else if (strcmp(command, "PMAP") == 0) {
			pmap(arena);
		} else if (strcmp(command, "MPROTECT") == 0) {
			parse_mprotect_command(arena, args);
		} else {
			print_err(INVALID_COMMAND);
		}

		if (err) {
			free(line);
			dealloc_arena(arena);
			exit(EXIT_FAILURE);
		}
	}
	free(line);
	return 0;
}
