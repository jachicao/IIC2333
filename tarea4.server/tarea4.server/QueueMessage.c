#include "QueueMessage.h"

QueueMessage* queue_message_create(void) {
    QueueMessage* queue = (QueueMessage*) shared_memory_alloc(sizeof(QueueMessage));
    queue -> front = NULL;
    queue -> rear = NULL;
    queue -> current_size = 0;
    return queue;
}

void queue_message_destroy(QueueMessage* queue) {
    shared_memory_free(queue, sizeof(QueueMessage));
}

bool queue_message_is_empty(QueueMessage* queue) {
    return queue -> current_size == 0;
}

void queue_message_enqueue(QueueMessage* queue, Message* message) {
    message -> prev = queue -> rear;
    if (queue_message_is_empty(queue)) {
        queue -> rear = message;
        queue -> front = message;
    } else {
        queue -> rear -> next = message;
        queue -> rear = message;
    }
    queue -> current_size++;
}

Message* queue_message_dequeue(QueueMessage* queue) {
    if (!queue_message_is_empty(queue)) {
        Message* message = queue -> front;
        queue -> front = message -> next;
        if (queue -> front != NULL) {
            queue -> front -> prev = NULL;
        }
        queue -> current_size--;
        return message;
    }
    return NULL;
}

Message* queue_message_peek(QueueMessage* queue) {
    return queue -> front;
}
