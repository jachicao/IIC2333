#include "Queue.h"

Queue* queue_create(int max_size) {
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue -> current_size = 0;
    queue -> max_size = max_size;
    queue -> front = NULL;
    queue -> rear = NULL;
    return queue;
}

void queue_destroy(Queue* queue) {
    // TODO
    /*
    Node* node = queue -> rear;
    while(node != NULL) {
        node_destroy(node);
        node = node -> prev;
    }
     */
    free(queue);
}

Node* queue_enqueue(Queue* queue, Node* node) {
    Node* prev = NULL;
    if (queue_is_full(queue)) {
        prev = queue_dequeue(queue);
    }
    node -> next = queue -> front;
    if (queue_is_empty(queue)) {
        queue -> rear = node;
        queue -> front = node;
    } else {
        queue -> front -> prev = node;
        queue -> front = node;
    }
    queue -> current_size++;
    return prev;
}

Node* queue_dequeue(Queue* queue) {
    if (!queue_is_empty(queue)) {
        if (queue -> front == queue -> rear) {
            queue -> front = NULL;
        }
        Node* temp = queue -> rear;
        queue -> rear = queue -> rear -> prev;
        
        if (queue -> rear != NULL) {
            queue -> rear -> next = NULL;
        }
        node_destroy(temp);
        queue -> current_size--;
    }
    return NULL;
}

bool queue_is_empty(Queue* queue) {
    return queue -> rear == NULL;
}

bool queue_is_full(Queue* queue) {
    return queue -> current_size == queue -> max_size;
}
