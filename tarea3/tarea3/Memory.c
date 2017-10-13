#include "Memory.h"

Memory* memory_create(enum policy_type policy, int offset_bits, int page_bits, int frame_bits, int tlb_size) {
    Memory* memory = (Memory*) malloc(sizeof(Memory));
    memory -> policy = policy;
    memory -> page_bits = page_bits;
    memory -> frame_bits = frame_bits;
    memory -> offset_bits = offset_bits;
    memory -> tlb = tlb_create(policy, tlb_size);
    return memory;
}

void memory_destroy(Memory* memory) {
    tlb_destroy(memory -> tlb);
    free(memory);
}

uint memory_read(Memory* memory, uint memory_address) {
    uint offset_mask = ((int)pow(2, memory -> offset_bits)) - 1;
    uint offset = memory_address & offset_mask;
    uint page_mask = ((int)pow(2, memory -> page_bits)) - 1;
    uint page = (memory_address & page_mask) >> memory -> offset_bits;
    uint frame = 0;
    if (tlb_contains_page(page)) {
        frame = tlb_get_frame(page);
        global_statistics -> tlb_hits++;
    } else {
        // ...
        tlb_add_page(page, frame);
    }
    global_statistics -> tlb_tries++;
    uint physical_address = (frame << memory -> frame_bits) | offset;
    return 0;
}
