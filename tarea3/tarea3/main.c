#include "main.h"

void on_destroy(int s) {
    keep_scanning = false;
    statistics_print(global_statistics);
    page_table_print(global_memory -> page_table);
    tlb_print(global_memory -> tlb);
    //physical_memory_print(global_memory -> physical_memory);
    if (global_memory != NULL) {
        memory_destroy(global_memory);
    }
    if (global_statistics != NULL) {
        statistics_destroy(global_statistics);
    }
    exit(EXIT_SUCCESS);
}

// 513 = 00000010 00000001
int main(int argc, const char * argv[]) {
    signal(SIGQUIT, on_destroy);
    signal(SIGINT, on_destroy);
    if (argc < 2) {
        printf("falta politica\n");
        return EXIT_FAILURE;
    }
    enum policy_type policy;
    if (strcmp(argv[1], "lru") == 0) {
        policy = LRU;
    } else if (strcmp(argv[1], "fifo") == 0) {
        policy = FIFO;
    } else {
        printf("politica desconocida\n");
        return EXIT_FAILURE;
    }
    global_statistics = statistics_create();
    global_memory = memory_create(policy);
    int memory_address;
    while(keep_scanning && scanf("%d", &memory_address) == 1) {
        printf("%d\n", memory_virtual_read(global_memory, memory_address));
    }
    on_destroy(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
