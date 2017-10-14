#ifndef PageTable_h
#define PageTable_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Constants.h"
#include "Enums.h"
#include "PhysicalMemory.h"
#include "Statistics.h"

extern Statistics* global_statistics;

typedef struct PageTableDictionary
{
    int size;
    int* array;
} PageTableDictionary;

PageTableDictionary* page_table_dictionary_create(int size);
void page_table_dictionary_destroy(PageTableDictionary* dictionary);
bool page_table_dictionary_contains(PageTableDictionary* dictionary, int key);
int page_table_dictionary_add(PageTableDictionary* dictionary, int key, int value);
int page_table_dictionary_remove(PageTableDictionary* dictionary, int key);
int page_table_dictionary_get(PageTableDictionary* dictionary, int key);

typedef struct PageTable
{
    PageTableDictionary* dictionary;
} PageTable;

PageTable* page_table_create(void);
void page_table_destroy(PageTable* page_table);
void page_table_add(PageTable* page_table, int page, int frame);
int page_table_get_from_page(PageTable* page_table, int page);
int page_table_get_from_frame(PageTable* page_table, int frame);
void page_table_remove_page(PageTable* page_table, int page);
void page_table_print(PageTable* page_table);

#endif /* PageTable_h */

