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

int page_table_get(PageTable* page_table, int page) {
    global_statistics -> page_tries++;
    if (page_table_dictionary_contains(page_table -> dictionary, page)) {
        return page_table_dictionary_get(page_table -> dictionary, page);
    } else {
        // page fault
        global_statistics -> page_faults++;
        printf("Swap in");
        FILE* file = fopen(DISK_FILE_NAME, "r");
        if (file != NULL) {
            int page_position = 0 + page * OFFSET_SIZE;
            fseek(file, page_position, SEEK_SET);
            unsigned char* bytes = (unsigned char*) malloc(OFFSET_SIZE);
            fread(&bytes, 1, OFFSET_SIZE, file);
            fclose(file);
            int frame = 0;
            //int frame = physical_memory_add(global_memory -> physical_memory, bytes);
            //page_table_dictionary_add(page_table -> dictionary, page, frame);
            return frame;
        } else {
            printf("Failed to read file");
            exit(1);
        }
    }
    return 0;
}

void page_table_remove_frame(PageTable* page_table, int frame) {
    int page = 0;
    for (int i = 0; i < page_table -> dictionary -> size; i++) {
        if (frame == page_table_dictionary_get(page_table -> dictionary, i)) {
            page = i;
            break;
        }
    }
    page_table_dictionary_remove(page_table -> dictionary, page);
}
