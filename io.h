#ifndef __IO_H
#define __IO_H

enum err_codes { INVALID_ALLOC_BLOCK, INVALID_COMMAND };

char *read_line();
int read_numbers(char *s, int nr, ...);

void print_err(enum err_codes err);

#endif //__IO_H