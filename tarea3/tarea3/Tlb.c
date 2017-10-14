#include "Tlb.h"

TlbNode* tlb_node_create(int page, int frame) {
    TlbNode* node = (TlbNode*) malloc(sizeof(TlbNode));
    node -> prev = NULL;
    node -> next = NULL;
    node -> page = page;
    node -> frame = frame;
    return node;
}

void tlb_node_destroy(TlbNode* node) {
    free(node);
}

TlbQueue* tlb_queue_create(int max_size) {
    TlbQueue* queue = (TlbQueue*) malloc(sizeof(TlbQueue));
    queue -> current_size = 0;
    queue -> max_size = max_size;
    queue -> front = NULL;
    queue -> rear = NULL;
    return queue;
}

void tlb_queue_destroy(TlbQueue* queue) {
    TlbNode* node = queue -> rear;
    while(node != NULL) {
        tlb_node_destroy(node);
        node = node -> prev;
    }
    free(queue);
}

bool tlb_queue_is_empty(TlbQueue* queue) {
    return queue -> rear == NULL;
}

bool tlb_queue_is_full(TlbQueue* queue) {
    return queue -> current_size == queue -> max_size;
}

TlbNode* tlb_queue_dequeue(TlbQueue* queue) {
    if (!tlb_queue_is_empty(queue)) {
        if (queue -> front == queue -> rear) {
            queue -> front = NULL;
        }
        TlbNode* temp = queue -> rear;
        queue -> rear = queue -> rear -> prev;
        
        if (queue -> rear != NULL) {
            queue -> rear -> next = NULL;
        }
        queue -> current_size--;
        return temp;
    }
    return NULL;
}

TlbNode* tlb_queue_enqueue(TlbQueue* queue, TlbNode* node) {
    TlbNode* prev = NULL;
    if (tlb_queue_is_full(queue)) {
        prev = tlb_queue_dequeue(queue);
    }
    node -> next = queue -> front;
    if (tlb_queue_is_empty(queue)) {
        queue -> rear = node;
        queue -> front = node;
    } else {
        queue -> front -> prev = node;
        queue -> front = node;
    }
    queue -> current_size++;
    return prev;
}

TlbDictionary* tlb_dictionary_create(int size) {
    TlbDictionary* dictionary = (TlbDictionary*) malloc(sizeof(TlbDictionary));
    dictionary -> size = size;
    dictionary -> array = (TlbNode**) malloc(dictionary -> size * sizeof(TlbNode*));
    for(int i = 0; i < dictionary -> size; i++) {
        dictionary -> array[i] = NULL;
    }
    return dictionary;
}

void tlb_dictionary_destroy(TlbDictionary* dictionary) {
    free(dictionary -> array);
    free(dictionary);
}

bool tlb_dictionary_contains(TlbDictionary* dictionary, int key) {
    return dictionary -> array[key] != NULL;
}

TlbNode* tlb_dictionary_add(TlbDictionary* dictionary, int key, TlbNode* node) {
    TlbNode* prev = dictionary -> array[key];
    dictionary -> array[key] = node;
    return prev;
}

TlbNode* tlb_dictionary_remove(TlbDictionary* dictionary, int key) {
    TlbNode* prev = dictionary -> array[key];
    dictionary -> array[key] = NULL;
    return prev;
}

TlbNode* tlb_dictionary_get(TlbDictionary* dictionary, int key) {
    return dictionary -> array[key];
}

Tlb* tlb_create(enum policy_type policy) {
    Tlb* tlb = (Tlb*) malloc(sizeof(Tlb));
    tlb -> policy = policy;
    tlb -> queue = tlb_queue_create(TLB_SIZE);
    tlb -> dictionary = tlb_dictionary_create(PAGE_SIZE);
    return tlb;
}

void tlb_destroy(Tlb* tlb) {
    tlb_queue_destroy(tlb -> queue);
    tlb_dictionary_destroy(tlb -> dictionary);
    free(tlb);
}

bool tlb_contains(Tlb* tlb, int page) {
    return tlb_dictionary_contains(tlb -> dictionary, page);
}

void tlb_add(Tlb* tlb, int page, int frame) {
    TlbNode* node = tlb_node_create(page, frame);
    TlbNode* prev = tlb_queue_enqueue(tlb -> queue, node);
    if (prev != NULL) {
        // full
        tlb_dictionary_remove(tlb -> dictionary, prev -> page);
        tlb_node_destroy(prev);
    }
    tlb_dictionary_add(tlb -> dictionary, node -> page, node);
}

int tlb_get(Tlb* tlb, int page) {
    TlbNode* node = tlb_dictionary_get(tlb -> dictionary, page);
    global_statistics -> tlb_tries++;
    if (node == NULL) {
        int frame = 0;
        //int frame = page_table_get(global_memory -> page_table, page);
        tlb_add(tlb, page, frame);
        return frame;
    } else if (node != tlb -> queue -> front) {
        if (tlb -> policy == LRU) {
            node -> prev -> next = node -> next;
            if (node -> next != NULL) {
                node -> next -> prev = node -> prev;
            }
            if (node == tlb -> queue -> rear) {
                tlb -> queue -> rear = node -> prev;
                tlb -> queue -> rear -> next = NULL;
            }
            node -> next = tlb -> queue -> front;
            node -> prev = NULL;
            
            node -> next -> prev = node;
            tlb -> queue -> front = node;
        }
    }
    global_statistics -> tlb_hits++;
    return node -> frame;
}
