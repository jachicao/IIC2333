#ifndef main_h
#define main_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include "Constants.h"
#include "Enums.h"
#include "Memory.h"
#include "Statistics.h"

Memory* global_memory = NULL;
Statistics* global_statistics = NULL;

bool keep_scanning = true;

int main(int argc, const char * argv[]);
void on_destroy(int s);

#endif /* main_h */
