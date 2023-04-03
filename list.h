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
 * Aloca un nou nod de lista, care contine informatia `data`.
 */
list_t *encapsulate(void *data);

/*
 * Returneaza ultimul element al listei `list` (sau NULL daca nu exista).
 */
list_t *last_item(list_t *list);

/*
 * Insereaza in `list` nodul `item` dupa `node.
 */
void insert_after(list_t **list, list_t *node, list_t *item);

void merge_lists(list_t *dest, list_t *source);

/*
 * Sterge `item` din `list`, refacand legaturile intre celelalte
 * noduri si actualizeaza capul listei daca este necesar.
 */
void remove_item(list_t **list, list_t *item);

/*
 * Elibereaza toate nodurile din `list`, aplicand `free_func(nod)`
 * pentru a elibera valorile stocate in noduri.
 */
void clear_list(list_t *list, void (*free_func)(void *item));

/*
 * Aplică funcția `func` cu argumentele `args` pe fiecare element din `list`.
 */
void apply_func(list_t *list, void (*func)(void *data, void *args), void *args);

#endif // __LIST_H
