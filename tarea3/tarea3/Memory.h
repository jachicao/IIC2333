#ifndef Memory_h
#define Memory_h

#include <stdlib.h>
#include <math.h>
#include "Enums.h"
#include "Tlb.h"
#include "Statistics.h"

typedef struct Memory
{
    enum policy_type policy;
    int page_bits;
    int frame_bits;
    int offset_bits;
    Tlb* tlb;
} Memory;

extern Memory* global_memory;
extern Statistics* global_statistics;

Memory* memory_create(enum policy_type policy, int offset_bits, int page_bits, int frame_bits, int tlb_size);
void memory_destroy(Memory* memory);
uint memory_read(Memory* memory, uint memory_address);

#endif /* Memory_h */
