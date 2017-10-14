#include "PhysicalMemory.h"

PhysicalMemoryNode* physical_memory_node_create(unsigned char* bytes) {
    PhysicalMemoryNode* node = (PhysicalMemoryNode*) malloc(sizeof(PhysicalMemoryNode));
    node -> prev = NULL;
    node -> next = NULL;
    node -> bytes = bytes;
    return node;
}

void physical_memory_node_destroy(PhysicalMemoryNode* node) {
    free(node -> bytes);
    free(node);
}

PhysicalMemoryQueue* physical_memory_queue_create(int max_size) {
    PhysicalMemoryQueue* queue = (PhysicalMemoryQueue*) malloc(sizeof(PhysicalMemoryQueue));
    queue -> current_size = 0;
    queue -> max_size = max_size;
    queue -> front = NULL;
    queue -> rear = NULL;
    return queue;
}

void physical_memory_queue_destroy(PhysicalMemoryQueue* queue) {
    PhysicalMemoryNode* node = queue -> front;
    while(node != NULL) {
        PhysicalMemoryNode* next = node -> next;
        physical_memory_node_destroy(node);
        node = next;
    }
    free(queue);
}

bool physical_memory_queue_is_empty(PhysicalMemoryQueue* queue) {
    return queue -> current_size == 0;
}

bool physical_memory_queue_is_full(PhysicalMemoryQueue* queue) {
    return queue -> current_size == queue -> max_size;
}

PhysicalMemoryNode* physical_memory_queue_enqueue(PhysicalMemoryQueue* queue, PhysicalMemoryNode* node, enum policy_type policy) {
    PhysicalMemoryNode* prev = NULL;
    if (physical_memory_queue_is_full(queue)) {
        if (policy == LRU) {
            prev = physical_memory_queue_remove_rear(queue);
        } else {
            prev = physical_memory_queue_remove_front(queue);
        }
    }
    node -> prev = queue -> rear;
    if (physical_memory_queue_is_empty(queue)) {
        queue -> rear = node;
        queue -> front = node;
    } else {
        queue -> rear -> next = node;
        queue -> rear = node;
    }
    queue -> current_size++;
    return prev;
}

void physical_memory_queue_put_at_front(PhysicalMemoryQueue* queue, PhysicalMemoryNode* node) {
    if (node != queue -> front) {
        node -> prev -> next = node -> next;
        if (node -> next != NULL) {
            node -> next -> prev = node -> prev;
        }
        if (node == queue -> rear) {
            queue -> rear = node -> prev;
            queue -> rear -> next = NULL;
        }
        node -> next = queue -> front;
        node -> prev = NULL;
        
        node -> next -> prev = node;
        queue -> front = node;
    }
}

PhysicalMemoryNode* physical_memory_queue_remove_front(PhysicalMemoryQueue* queue) {
    if (!physical_memory_queue_is_empty(queue)) {
        PhysicalMemoryNode* temp = queue -> front;
        queue -> front = queue -> front -> next;
        
        if (queue -> front != NULL) {
            queue -> front -> prev = NULL;
        }
        queue -> current_size--;
        return temp;
    }
    return NULL;
}

PhysicalMemoryNode* physical_memory_queue_remove_rear(PhysicalMemoryQueue* queue) {
    if (!physical_memory_queue_is_empty(queue)) {
        PhysicalMemoryNode* temp = queue -> rear;
        queue -> rear = queue -> rear -> prev;
        
        if (queue -> rear != NULL) {
            queue -> rear -> next = NULL;
        }
        queue -> current_size--;
        return temp;
    }
    return NULL;
}

PhysicalMemoryDictionary* physical_memory_dictionary_create(int size) {
    PhysicalMemoryDictionary* dictionary = (PhysicalMemoryDictionary*) malloc(sizeof(PhysicalMemoryDictionary));
    dictionary -> size = size;
    dictionary -> array = (PhysicalMemoryNode**) malloc(dictionary -> size * sizeof(PhysicalMemoryNode*));
    for(int i = 0; i < dictionary -> size; i++) {
        dictionary -> array[i] = NULL;
    }
    return dictionary;
}

void physical_memory_dictionary_destroy(PhysicalMemoryDictionary* dictionary) {
    free(dictionary -> array);
    free(dictionary);
}

bool physical_memory_dictionary_contains(PhysicalMemoryDictionary* dictionary, int key) {
    return dictionary -> array[key] != NULL;
}

PhysicalMemoryNode* physical_memory_dictionary_add(PhysicalMemoryDictionary* dictionary, int key, PhysicalMemoryNode* node) {
    PhysicalMemoryNode* prev = dictionary -> array[key];
    dictionary -> array[key] = node;
    return prev;
}

PhysicalMemoryNode* physical_memory_dictionary_remove(PhysicalMemoryDictionary* dictionary, int key) {
    PhysicalMemoryNode* prev = dictionary -> array[key];
    dictionary -> array[key] = NULL;
    return prev;
}

PhysicalMemoryNode* physical_memory_dictionary_get(PhysicalMemoryDictionary* dictionary, int key) {
    return dictionary -> array[key];
}

PhysicalMemory* physical_memory_create(enum policy_type policy) {
    PhysicalMemory* physical_memory = (PhysicalMemory*) malloc(sizeof(PhysicalMemory));
    physical_memory -> policy = policy;
    physical_memory -> queue = physical_memory_queue_create(FRAME_SIZE);
    physical_memory -> dictionary = physical_memory_dictionary_create(PAGE_SIZE);
    return physical_memory;
}

void physical_memory_destroy(PhysicalMemory* physical_memory) {
    physical_memory_queue_destroy(physical_memory -> queue);
    physical_memory_dictionary_destroy(physical_memory -> dictionary);
    free(physical_memory);
}

int physical_memory_add(PhysicalMemory* physical_memory, unsigned char* bytes, int* prev_frame) {
    int frame = physical_memory -> queue -> current_size;
    PhysicalMemoryNode* node = physical_memory_node_create(bytes);
    PhysicalMemoryNode* prev = physical_memory_queue_enqueue(physical_memory -> queue, node, physical_memory -> policy);
    if (prev != NULL) {
        // full
        frame = prev -> frame;
        (*prev_frame) = frame;
        physical_memory_dictionary_remove(physical_memory -> dictionary, frame);
        physical_memory_node_destroy(prev);
        // swap out
    }
    node -> frame = frame;
    physical_memory_dictionary_add(physical_memory -> dictionary, node -> frame, node);
    if (physical_memory -> policy == LRU) {
        physical_memory_queue_put_at_front(physical_memory -> queue, node);
    }
    return node -> frame;
}

char physical_memory_get(PhysicalMemory* physical_memory, int frame, int offset) {
    PhysicalMemoryNode* node = physical_memory_dictionary_get(physical_memory -> dictionary, frame);
    if (node == NULL) {
        //PAGE FAULT
        printf("Page fault, this shouldnt happen");
    } else if (physical_memory -> policy == LRU) {
        physical_memory_queue_put_at_front(physical_memory -> queue, node);
    }
    return node -> bytes[offset];
}

void physical_memory_print(PhysicalMemory* physical_memory) {
    printf("PhysicalMemory\n");
    printf("i\tframe\n");
    int counter = 0;
    PhysicalMemoryNode* node = physical_memory -> queue -> front;
    while (node != NULL) {
        printf("%d\t%d\n", counter, node -> frame);
        node = node -> next;
        counter++;
    }
}
