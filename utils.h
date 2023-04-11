/*
 * Copyright: Sima Alexandru (312CA) 2023
 */
#ifndef __UTILS_H
#define __UTILS_H

#include <stddef.h>
#include <stdint.h>

/*
 * Calculeaza minimul a 2 numere.
 */
uint64_t min(const uint64_t a, const uint64_t b);

/*
 * Returneaza lungimea stringului `str`, cautand
 * in maximum `maxsize` caractere.
 */
size_t max_len(char *const str, const uint64_t maxsize);

#endif // __UTILS_H
