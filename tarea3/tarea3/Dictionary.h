#ifndef Dictionary_h
#define Dictionary_h

#include "Node.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct Dictionary
{
    int size;
    Node** array;
} Dictionary;

Dictionary* dictionary_create(int size);
void dictionary_destroy(Dictionary* dictionary);
bool dictionary_contains(Dictionary* dictionary, int key);
Node* dictionary_add(Dictionary* dictionary, int key, Node* node);
Node* dictionary_remove(Dictionary* dictionary, int key);


#endif /* Dictionary_h */
