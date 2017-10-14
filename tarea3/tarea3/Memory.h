#ifndef Memory_h
#define Memory_h

#include <stdio.h>
#include <stdlib.h>
#include "Constants.h"
#include "Enums.h"
#include "PageTable.h"
#include "Tlb.h"
#include "PhysicalMemory.h"

typedef struct Memory
{
    enum policy_type policy;
    PageTable* page_table;
    Tlb* tlb;
    PhysicalMemory* physical_memory;
} Memory;

Memory* memory_create(enum policy_type policy);
void memory_destroy(Memory* memory);
unsigned char memory_virtual_read(Memory* memory, int memory_address);

#endif /* Memory_h */

