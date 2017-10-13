#ifndef Queue_h
#define Queue_h

#include "Node.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct Queue
{
    int current_size;
    int max_size;
    Node* front;
    Node* rear;
} Queue;

Queue* queue_create(int max_size);
void queue_destroy(Queue* queue);
Node* queue_enqueue(Queue* queue, Node* node);
Node* queue_dequeue(Queue* queue);
bool queue_is_empty(Queue* queue);
bool queue_is_full(Queue* queue);

#endif /* Queue_h */
