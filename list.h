/*
 * Operatii pe liste dublu inlantuite generice.
 */
#ifndef __LIST_H
#define __LIST_H

#include <stdint.h>

typedef struct list_t list_t;
struct list_t {
	void *data;
	list_t *prev;
	list_t *next;
};

/*
 * Sterge `item` din `list`, refacand legaturile intre celelalte
 * noduri si actualizeaza capul listei daca este necesar. 
 */
void remove_item(list_t **list, list_t *item);

/*
 * Elibereaza toate nodurile din `list`, aplicand `free_func(nod)`
 * pentru a elibera valorile stocate in noduri. 
 */
void clear_list(list_t *list, void (*free_func)(void *data));

/*
 * Aplica functia `print_func(index, nod)` pentru
 * fiecare element din lista `list`.
 */
void print_list(list_t *list, void (*print_func)(uint64_t index, void *data));

#endif // __LIST_H