#include "Memory.h"

Memory* memory_create(enum policy_type policy) {
    Memory* memory = (Memory*) malloc(sizeof(Memory));
    memory -> policy = policy;
    memory -> physical_memory = physical_memory_create(policy);
    memory -> page_table = page_table_create();
    memory -> tlb = tlb_create(policy);
    return memory;
}

void memory_destroy(Memory* memory) {
    physical_memory_destroy(memory -> physical_memory);
    page_table_destroy(memory -> page_table);
    tlb_destroy(memory -> tlb);
    free(memory);
}
int memory_virtual_read(Memory* memory, int memory_address) {
    return 0;
}
