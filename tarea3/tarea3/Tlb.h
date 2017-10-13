#ifndef Tlb_h
#define Tlb_h

#include <stdlib.h>
#include <stdbool.h>
#include "Enums.h"

typedef struct Tlb
{
    enum policy_type policy;
    int size;
} Tlb;

Tlb* tlb_create(enum policy_type policy, int size);
void tlb_destroy(Tlb* tlb);
bool tlb_contains_page(uint page);
uint tlb_get_frame(uint page);
void tlb_add_page(uint page, uint frame);


#endif /* Tlb_h */
