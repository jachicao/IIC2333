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
unsigned char memory_virtual_read(Memory* memory, int memory_address) {
    int offset = memory_address & OFFSET_MASK;
    int page = (memory_address >> OFFSET_BITS) & PAGE_MASK;
    //printf("Offset: %d, page: %d\n", offset, page);
    int frame = tlb_get(memory -> tlb, page);
    global_statistics -> tlb_tries++;
    global_statistics -> page_tries++;
    unsigned char* bytes = NULL;
    if (frame == -1) {
        //hit miss
        //printf("Hit miss\n");
        frame = page_table_get_from_page(memory -> page_table, page);
        if (frame == -1) {
            global_statistics -> page_faults++;
            //printf("Page fault\n");
            // page fault
            FILE* file = fopen(DISK_FILE_NAME, "r");
            if (file != NULL) {
                int page_position = 0 + page * OFFSET_SIZE;
                fseek(file, page_position, SEEK_SET);
                bytes = (unsigned char*) malloc(OFFSET_SIZE);
                fread(bytes, 1, OFFSET_SIZE, file);
                fclose(file);
                int prev_frame = -1;
                frame = physical_memory_add(memory -> physical_memory, bytes, &prev_frame);
                if (prev_frame > -1) {
                    // frame replaced
                    int prev_page = page_table_get_from_frame(memory -> page_table, prev_frame);
                    //printf("Remove page: %d, frame: %d\n", prev_page, prev_frame);
                    page_table_remove_page(memory -> page_table, prev_page);
                    tlb_remove(memory -> tlb, prev_page);
                }
                page_table_add(memory -> page_table, page, frame);
                tlb_add(memory -> tlb, page, frame);
                return bytes[offset];
            } else {
                printf("Failed to read file");
                exit(1);
            }
        } else {
            tlb_add(memory -> tlb, page, frame);
        }
    } else {
        global_statistics -> tlb_hits++;
    }
    return physical_memory_get(memory -> physical_memory, frame, offset);
}
