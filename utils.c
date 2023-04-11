/*
 * Copyright: Sima Alexandru (312CA) 2023
 */
#include "utils.h"

uint64_t min(const uint64_t a, const uint64_t b)
{
	return a < b ? a : b;
}

size_t max_len(char *const str, const size_t maxsize)
{
	for (size_t i = 0; i < maxsize; ++i)
		if (!str[i])
			return i;
	return maxsize;
}
