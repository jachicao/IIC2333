#include "main.h"

// 513 = 00000010 00000001
int main(int argc, const char * argv[]) {
    signal(SIGQUIT, on_sigquit);
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
    global_memory = memory_create(policy, OFFSET_BITS, PAGE_BITS, FRAME_BITS, TLB_SIZE);
    uint memory_address;
    while(keep_scanning && scanf("%d", &memory_address) == 1) {
        printf("memory: %d\n", memory_address);
    }
    main_destroy();
    return EXIT_SUCCESS;
}

void on_sigquit(int s) {
    keep_scanning = false;
}

void main_destroy() {
    if (global_memory != NULL) {
        memory_destroy(global_memory);
    }
    if (global_statistics != NULL) {
        statistics_destroy(global_statistics);
    }
}
