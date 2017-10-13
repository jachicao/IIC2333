#include "Node.h"

Node* node_create(unsigned pageNumber) {
    Node* node = (Node*) malloc(sizeof(Node));
    node -> prev = NULL;
    node -> next = NULL;
    return node;
}

void node_destroy(Node* node) {
    free(node);
}
