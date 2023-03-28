#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "mem_io.h"
#include "mem_prot.h"
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
			u64 size;
			if (*read_numbers(args, 1, &size) == '\0')
				arena = alloc_arena(size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "DEALLOC_ARENA") == 0) {
			dealloc_arena(arena);
			free(line);
			exit(EXIT_SUCCESS); // TODO
		} else if (strcmp(command, "ALLOC_BLOCK") == 0) {
			u64 address, size;
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
			u64 address;
			if (*read_numbers(args, 1, &address) == '\0')
				free_block(arena, address);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "READ") == 0) {
			u64 address, size;
			if (*read_numbers(args, 2, &address, &size) == '\0')
				read(arena, address, size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "WRITE") == 0) {
			u64 address, size;
			char *data_begin = read_numbers(args, 2, &address, &size) + 1;
			u8 *buffer = malloc(sizeof(u8) * size);
			// TODO if (!buffer) return 1;
			u64 bytes_read = strlen(data_begin);
			memcpy(buffer, data_begin, bytes_read);
			// TODO
			// fread(buffer + bytes_read, sizeof(char), size - bytes_read,
			// stdin);
			write(arena, address, size, buffer);
			free(buffer);
		} else if (strcmp(command, "PMAP") == 0) {
			pmap(arena);
		} else if (strcmp(command, "MPROTECT") == 0) {
			u64 address;
			char *perm_str = read_numbers(args, 1, &address);
			u8 perm = parse_perm_str(perm_str);
			mprotect(arena, address, perm);
		} else {
			print_err(INVALID_COMMAND);
		}
	}
	free(line);
	return 0;
}
