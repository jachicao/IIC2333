#ifndef PhysicalMemory_h
#define PhysicalMemory_h

#include <stdlib.h>
#include <stdbool.h>
#include "PageTable.h"

typedef struct PhysicalMemoryNode
{
    struct PhysicalMemoryNode* prev;
    struct PhysicalMemoryNode* next;
    int frame;
    unsigned char* bytes;
} PhysicalMemoryNode;

PhysicalMemoryNode* physical_memory_node_create(unsigned char* bytes);
void physical_memory_node_destroy(PhysicalMemoryNode* node);

typedef struct PhysicalMemoryQueue
{
    int current_size;
    int max_size;
    PhysicalMemoryNode* front;
    PhysicalMemoryNode* rear;
} PhysicalMemoryQueue;

PhysicalMemoryQueue* physical_memory_queue_create(int max_size);
void physical_memory_queue_destroy(PhysicalMemoryQueue* queue);
bool physical_memory_queue_is_empty(PhysicalMemoryQueue* queue);
bool physical_memory_queue_is_full(PhysicalMemoryQueue* queue);
PhysicalMemoryNode* physical_memory_queue_dequeue(PhysicalMemoryQueue* queue);
PhysicalMemoryNode* physical_memory_queue_enqueue(PhysicalMemoryQueue* queue, PhysicalMemoryNode* node);

typedef struct PhysicalMemoryDictionary
{
    int size;
    PhysicalMemoryNode** array;
} PhysicalMemoryDictionary;

PhysicalMemoryDictionary* physical_memory_dictionary_create(int size);
void physical_memory_dictionary_destroy(PhysicalMemoryDictionary* dictionary);
bool physical_memory_dictionary_contains(PhysicalMemoryDictionary* dictionary, int key);
PhysicalMemoryNode* physical_memory_dictionary_add(PhysicalMemoryDictionary* dictionary, int key, PhysicalMemoryNode* node);
PhysicalMemoryNode* physical_memory_dictionary_remove(PhysicalMemoryDictionary* dictionary, int key);
PhysicalMemoryNode* physical_memory_dictionary_get(PhysicalMemoryDictionary* dictionary, int key);

typedef struct PhysicalMemory
{
    enum policy_type policy;
    PhysicalMemoryQueue* queue;
    PhysicalMemoryDictionary* dictionary;
    struct Memory* memory;
} PhysicalMemory;

PhysicalMemory* physical_memory_create(enum policy_type policy);
void physical_memory_destroy(PhysicalMemory* physical_memory);
bool physical_memory_contains(PhysicalMemory* physical_memory, int frame);
int physical_memory_add(PhysicalMemory* physical_memory, unsigned char* bytes);
char physical_memory_get(PhysicalMemory* physical_memory, int frame, int offset);

#endif /* PhysicalMemory_h */

