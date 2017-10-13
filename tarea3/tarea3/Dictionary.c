#include "Dictionary.h"

Dictionary* dictionary_create(int size) {
    Dictionary* dictionary = (Dictionary*) malloc(sizeof(Dictionary));
    dictionary -> size = size;
    dictionary -> array = (Node**) malloc(dictionary -> size * sizeof(Node*));
    for(int i = 0; i < dictionary -> size; i++) {
        dictionary -> array[i] = NULL;
    }
    return dictionary;
}

void dictionary_destroy(Dictionary* dictionary) {
    free(dictionary -> array);
    free(dictionary);
}

bool dictionary_contains(Dictionary* dictionary, int key) {
    return dictionary -> array[key] != NULL;
}

Node* dictionary_add(Dictionary* dictionary, int key, Node* node) {
    Node* prev = dictionary -> array[key];
    dictionary -> array[key] = node;
    return prev;
}

Node* dictionary_remove(Dictionary* dictionary, int key) {
    Node* prev = dictionary -> array[key];
    dictionary -> array[key] = NULL;
    return prev;
}
