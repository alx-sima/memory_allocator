#include <stdint.h>
#include <stdlib.h>

#include "list.h"

list_t *encapsulate(void *data)
{
	list_t *node = malloc(sizeof(list_t));
	if (!node)
		return NULL;

	node->data = data;
	node->prev = NULL;
	node->next = NULL;

	return node;
}

list_t *last_item(list_t *list)
{
	if (!list)
		return NULL;

	while (list->next)
		list = list->next;

	return list;
}

void insert_after(list_t **list, list_t *node, list_t *item)
{
	if (!node) {
		item->next = *list;
		item->prev = NULL;
		if (*list)
			(*list)->prev = item;
		*list = item;
		return;
	}

	item->prev = node;
	item->next = node->next;
	if (item->next)
		item->next->prev = item;
	node->next = item;
}

void merge_lists(list_t *dest, list_t *source)
{
	dest = last_item(dest);
	if (!dest || !source)
		return;

	dest->next = source;
	source->prev = dest;
}

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
