#include <stdint.h>
#include <stdlib.h>

#include "list.h"

void remove_item(list_t **list, list_t *item)
{
	list_t *next = item->next;
	list_t *prev = item->prev;

	if (prev)
		prev->next = next;
	else
		*list = next;

	if (next)
		next->prev = prev;
}

void clear_list(list_t *list, void (*free_func)(void *))
{
	while (list) {
		list_t *next = list->next;
		free_func(list->data);
		free(list);
		list = next;
	}
}

void print_list(list_t *list, void (*print_func)(uint64_t, void *))
{
	for (uint64_t i = 1; list != NULL; ++i) {
		print_func(i, list->data);
		list = list->next;
	}
}