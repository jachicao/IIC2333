#ifndef main_h
#define main_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include "Memory.h"
#include "Statistics.h"

#define OFFSET_BITS 8
#define PAGE_BITS 8
#define FRAME_BITS 8
#define TLB_SIZE 32

Memory* global_memory = NULL;
Statistics* global_statistics = NULL;

bool keep_scanning = true;

int main(int argc, const char * argv[]);
void on_sigquit(int s);
void main_destroy();

#endif /* main_h */
