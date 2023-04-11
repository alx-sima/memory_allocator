/*
 * Operatii de alocare/eliberare a blocurilor si miniblocurilor.
 * Copyright: Sima Alexandru (312CA) 2023
 */
#ifndef __MEM_ALLOC_H
#define __MEM_ALLOC_H

#include "vma.h"

/*
 * Initializeaza un bloc nou la adresa `address` si de dimensiune `size`.
 */
block_t  *init_block(const uint64_t address, const uint64_t size);

/*
 * Elibereaza miniblocul din `data`.
 */
void free_miniblock_data(void *data);

/*
 * Elibereaza blocul din `data` si toate
 * resursele asociate (toate miniblocurile).
 */
void free_block_data(void *data);

#endif // __MEM_ALOC_H
