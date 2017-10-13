#ifndef Node_h
#define Node_h

#include <stdlib.h>

typedef struct Node
{
    struct Node* prev;
    struct Node* next;
    int pageNumber; // the page number stored in this Node
} Node;

Node* node_create(unsigned pageNumber);
void node_destroy(Node* node);

#endif /* Node_h */
