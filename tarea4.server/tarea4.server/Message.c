#include "Message.h"

Message* message_create(int socket, enum MessageType type, int payload_size, byte_t* payload) {
    Message* message = (Message*) shared_memory_alloc(sizeof(Message));
    message -> socket = socket;
    message -> type = type;
    message -> payload_size = payload_size;
    message -> payload = NULL;
    if (message -> payload_size > 0) {
        message -> payload = (byte_t*) shared_memory_alloc(message -> payload_size * sizeof(byte_t));
        memcpy(message -> payload, payload, message -> payload_size);
    }
    message -> prev = NULL;
    message -> next = NULL;
    return message;
}

void message_destroy(Message* message) {
    if (message -> payload_size > 0) {
        shared_memory_free(message, message -> payload_size * sizeof(byte_t));
    }
    shared_memory_free(message, sizeof(Message));
}
