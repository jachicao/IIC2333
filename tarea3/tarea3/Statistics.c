#include "Statistics.h"

Statistics* statistics_create() {
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
