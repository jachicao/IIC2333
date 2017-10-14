#include "Statistics.h"

Statistics* statistics_create(void) {
    Statistics* statistics = (Statistics*) malloc(sizeof(Statistics));
    statistics -> tlb_hits = 0;
    statistics -> tlb_tries = 0;
    statistics -> page_tries = 0;
    statistics -> page_faults = 0;
    return statistics;
}

void statistics_destroy(Statistics* statistics) {
    free(statistics);
}

void statistics_print(Statistics* statistics) {
    float fault_percentage = 100 * ((double)statistics -> page_faults) / ((double)statistics -> page_tries);
    printf("PORCENTAJE_PAGE_FAULTS = %f", fault_percentage);
    printf("%%");
    printf("\n");
    float hit_percentage = 100 * ((double)statistics -> tlb_hits) / ((double)statistics -> tlb_tries);
    printf("PORCENTAJE_TLB_HITS = %f", hit_percentage);
    printf("%%");
    printf("\n");
}
