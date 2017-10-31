#ifndef Message_h
#define Message_h

#include <string.h>
#include <stdint.h>

#include "MessageType.h"
#include "SharedMemory.h"


typedef uint8_t byte_t;

typedef struct Message
{
    int socket;
    enum MessageType type;
    int payload_size;
    byte_t* payload;
    
    struct Message* prev;
    struct Message* next;
} Message;

Message* message_create(int socket, enum MessageType type, int payload_size, byte_t* payload);

void message_destroy(Message* message);

#endif /* Message_h */
