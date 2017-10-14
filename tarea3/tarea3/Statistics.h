#ifndef Statistics_h
#define Statistics_h

#include <stdio.h>
#include <stdlib.h>

typedef struct Statistics
{
    int tlb_hits;
    int tlb_tries;
    int page_faults;
    int page_tries;
} Statistics;

Statistics* statistics_create(void);
void statistics_destroy(Statistics* statistics);
void statistics_print(Statistics* statistics);

#endif /* Statistics_h */
