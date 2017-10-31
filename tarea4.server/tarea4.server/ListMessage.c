#include "ListMessage.h"

ListMessage* list_message_create(void) {
    ListMessage* list_message = (ListMessage*) shared_memory_alloc(sizeof(ListMessage));
    list_message -> head = NULL;
    list_message -> tail = NULL;
    list_message -> current_size = 0;
    return list_message;
}

void list_message_destroy(ListMessage* list_message) {
    shared_memory_free(list_message, sizeof(ListMessage));
}

bool list_message_is_empty(ListMessage* list_message) {
    return list_message -> current_size == 0;
}

void list_message_insert(ListMessage* list_message, Message* message) {
    message -> prev = list_message -> tail;
    if (list_message_is_empty(list_message)) {
        list_message -> head = message;
        list_message -> tail = message;
    } else {
        list_message -> tail -> next = message;
        list_message -> tail = message;
    }
    list_message -> current_size++;
}

void list_message_remove(ListMessage* list_message, Message* message) {
    if (!list_message_is_empty(list_message)) {
        if (message == list_message -> tail) {
            list_message -> tail = message -> prev;
        }
        if (message == list_message -> head) {
            list_message -> head = message -> next;
        }
        if (message -> prev != NULL) {
            message -> prev -> next = message -> next;
        }
        if (message -> next != NULL) {
            message -> next -> prev = message -> prev;
        }
        list_message -> current_size--;
    }
}
