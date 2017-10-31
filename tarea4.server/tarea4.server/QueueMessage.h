#ifndef QueueMessage_h
#define QueueMessage_h

#include <stdbool.h>

#include "Message.h"

typedef struct QueueMessage
{
    int current_size;
    Message* front;
    Message* rear;
} QueueMessage;

QueueMessage* queue_message_create(void);

void queue_message_destroy(QueueMessage* queue);

bool queue_message_is_empty(QueueMessage* queue);

void queue_message_enqueue(QueueMessage* queue, Message* message);

Message* queue_message_dequeue(QueueMessage* queue);

Message* queue_message_peek(QueueMessage* queue);

#endif /* QueueMessage_h */
