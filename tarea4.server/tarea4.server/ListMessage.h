#ifndef ListMessage_h
#define ListMessage_h

#include <stdbool.h>

#include "Message.h"

typedef struct ListMessage
{
    int current_size;
    Message* head;
    Message* tail;
} ListMessage;

ListMessage* list_message_create(void);

void list_message_destroy(ListMessage* list_message);

bool list_message_is_empty(ListMessage* list_message);

void list_message_insert(ListMessage* list_message, Message* message);

void list_message_remove(ListMessage* list_message, Message* message);

#endif /* ListMessage_h */
