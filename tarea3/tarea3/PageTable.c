#include "PageTable.h"

PageTableDictionary* page_table_dictionary_create(int size) {
    PageTableDictionary* dictionary = (PageTableDictionary*) malloc(sizeof(PageTableDictionary));
    dictionary -> size = size;
    dictionary -> array = (int*) malloc(dictionary -> size * sizeof(int));
    for(int i = 0; i < dictionary -> size; i++) {
        dictionary -> array[i] = -1;
    }
    return dictionary;
}

void page_table_dictionary_destroy(PageTableDictionary* dictionary) {
    free(dictionary -> array);
    free(dictionary);
}

bool page_table_dictionary_contains(PageTableDictionary* dictionary, int key) {
    return dictionary -> array[key] > -1;
}

int page_table_dictionary_add(PageTableDictionary* dictionary, int key, int value) {
    int prev = dictionary -> array[key];
    dictionary -> array[key] = value;
    return prev;
}

int page_table_dictionary_remove(PageTableDictionary* dictionary, int key) {
    int prev = dictionary -> array[key];
    dictionary -> array[key] = -1;
    return prev;
}

int page_table_dictionary_get(PageTableDictionary* dictionary, int key) {
    return dictionary -> array[key];
}

PageTable* page_table_create(void) {
    PageTable* page_table = (PageTable*) malloc(sizeof(PageTable));
    page_table -> dictionary = page_table_dictionary_create(PAGE_SIZE);
    return page_table;
}

void page_table_destroy(PageTable* page_table) {
    page_table_dictionary_destroy(page_table -> dictionary);
    free(page_table);
}

void page_table_add(PageTable* page_table, int page, int frame) {
    page_table_dictionary_add(page_table -> dictionary, page, frame);
}

int page_table_get_from_page(PageTable* page_table, int page) {
    if (page_table_dictionary_contains(page_table -> dictionary, page)) {
        return page_table_dictionary_get(page_table -> dictionary, page);
    } else {
        return -1;
    }
}

int page_table_get_from_frame(PageTable* page_table, int frame) {
    for (int i = 0; i < page_table -> dictionary -> size; i++) {
        if (frame == page_table_dictionary_get(page_table -> dictionary, i)) {
            return i;
        }
    }
    return -1;
}

void page_table_remove_page(PageTable* page_table, int page) {
    page_table_dictionary_remove(page_table -> dictionary, page);
}

void page_table_print(PageTable* page_table) {
    printf("TABLA DE PAGINAS\n");
    printf("page\tframe\n");
    for (int i = 0; i < page_table -> dictionary -> size; i++) {
        if (page_table_dictionary_contains(page_table -> dictionary, i)) {
            printf("%d\t%d\n", i, page_table_dictionary_get(page_table -> dictionary, i));
        } else {
            printf("%d\n", i);
        }
    }
}
