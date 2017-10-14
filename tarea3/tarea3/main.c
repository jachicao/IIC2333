#include "main.h"

void on_destroy(int s) {
    keep_scanning = false;
    float fault_percentage = 100 * ((double)global_statistics -> page_faults) / ((double)global_statistics -> page_tries);
    printf("PORCENTAJE_PAGE_FAULTS = %f", fault_percentage);
    printf("%%");
    printf("\n");
    float hit_percentage = 100 * ((double)global_statistics -> tlb_hits) / ((double)global_statistics -> tlb_tries);
    printf("PORCENTAJE_TLB_HITS = %f", hit_percentage);
    printf("%%");
    printf("\n");
    printf("TABLA DE PAGINAS\n");
    printf("page\tframe\n");
    for (int i = 0; i < global_memory -> page_table -> dictionary -> size; i++) {
        if (page_table_dictionary_contains(global_memory -> page_table -> dictionary, i)) {
            printf("%d\t%d\n", i, page_table_dictionary_get(global_memory -> page_table -> dictionary, i));
        } else {
            printf("%d\n", i);
        }
    }
    printf("TLB\n");
    printf("i\tpage\tframe\n");
    int counter = 0;
    TlbNode* node = global_memory -> tlb -> queue -> front;
    while (node != NULL) {
        printf("%d\t%d\t%d\n", counter, node -> page, node -> frame);
        node = node -> next;
        counter++;
    }
    for (int i = counter; i < global_memory -> tlb -> queue -> max_size; i++) {
        printf("%d\n", i);
    }
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
