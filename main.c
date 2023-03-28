#include <stddef.h>
#include <stdint.h>
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
		uint64_t address, size;
		char *command = strtok(line, "\n ");
		if (!command)
			continue;
		char *args = strtok(NULL, "");
		if (strcmp(command, "ALLOC_ARENA") == 0) {
			if (*read_numbers(args, 1, &size) == '\n')
				arena = alloc_arena(size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "DEALLOC_ARENA") == 0) {
			dealloc_arena(arena);
			free(line);
			exit(EXIT_SUCCESS); // TODO
		} else if (strcmp(command, "ALLOC_BLOCK") == 0) {
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
		} else if (strcmp(command, "FREE_BLOCK") == 0) {
			if (*read_numbers(args, 1, &address) == '\n')
				free_block(arena, address);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "READ") == 0) {
			if (*read_numbers(args, 2, &address, &size) == '\n')
				read(arena, address, size);
			else
				print_err(INVALID_COMMAND);
		} else if (strcmp(command, "WRITE") == 0) {
			char *data_begin = read_numbers(args, 2, &address, &size);
			if (*data_begin == ' ')
				++data_begin;
			uint8_t *buffer = malloc(sizeof(uint8_t) * size);
			// TODO if (!buffer) return 1;
			uint64_t batch = min(size, strlen(data_begin));
			memcpy(buffer, data_begin, batch);
			uint64_t bytes_read = batch;
			while (bytes_read < size) {
				size_t line_length = read_line(&line, &line_size);
				batch = min(size - bytes_read, line_length);
				memcpy(buffer + bytes_read, line, batch);
				bytes_read += batch;
			}
			write(arena, address, size, buffer);
			if (line[batch] != '\n') {
				char *tmp = strtok(data_begin + batch, "\n ");
				while (tmp) {
					print_err(INVALID_COMMAND);
					tmp = strtok(NULL, "\n ");
				}
			}
			free(buffer);
		} else if (strcmp(command, "PMAP") == 0) {
			pmap(arena);
		} else if (strcmp(command, "MPROTECT") == 0) {
			char *perm_str = read_numbers(args, 1, &address);
			mprotect(arena, address, (int8_t *)perm_str);
		} else {
			print_err(INVALID_COMMAND);
		}
	}
	free(line);
	return 0;
}
