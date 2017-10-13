#include "Tlb.h"

Tlb* tlb_create(enum policy_type policy, int size) {
    Tlb* tlb = (Tlb*) malloc(sizeof(Tlb));
    tlb -> policy = policy;
    tlb -> size = size;
    return tlb;
}

void tlb_destroy(Tlb* tlb) {
    free(tlb);
}

bool tlb_contains_page(uint page) {
    return false;
}

uint tlb_get_frame(uint page) {
    return 0;
}

void tlb_add_page(uint page, uint frame) {
    
}
