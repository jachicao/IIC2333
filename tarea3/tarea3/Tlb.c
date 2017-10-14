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
    TlbNode* node = queue -> front;
    while(node != NULL) {
        TlbNode* next = node -> next;
        tlb_node_destroy(node);
        node = next;
    }
    free(queue);
}

bool tlb_queue_is_empty(TlbQueue* queue) {
    return queue -> current_size == 0;
}

bool tlb_queue_is_full(TlbQueue* queue) {
    return queue -> current_size == queue -> max_size;
}

TlbNode* tlb_queue_enqueue(TlbQueue* queue, TlbNode* node, enum policy_type policy) {
    TlbNode* prev = NULL;
    if (tlb_queue_is_full(queue)) {
        if (policy == LRU) {
            prev = tlb_queue_remove_rear(queue);
        } else {
            prev = tlb_queue_remove_front(queue);
        }
    }
    node -> prev = queue -> rear;
    if (tlb_queue_is_empty(queue)) {
        queue -> rear = node;
        queue -> front = node;
    } else {
        queue -> rear -> next = node;
        queue -> rear = node;
    }
    queue -> current_size++;
    return prev;
}

void tlb_queue_put_at_front(TlbQueue* queue, TlbNode* node) {
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

TlbNode* tlb_queue_remove_front(TlbQueue* queue) {
    if (!tlb_queue_is_empty(queue)) {
        TlbNode* temp = queue -> front;
        queue -> front = temp -> next;
        
        if (queue -> front != NULL) {
            queue -> front -> prev = NULL;
        }
        queue -> current_size--;
        return temp;
    }
    return NULL;
}

TlbNode* tlb_queue_remove_rear(TlbQueue* queue) {
    if (!tlb_queue_is_empty(queue)) {
        TlbNode* temp = queue -> rear;
        queue -> rear = temp -> prev;
        
        if (queue -> rear != NULL) {
            queue -> rear -> next = NULL;
        }
        queue -> current_size--;
        return temp;
    }
    return NULL;
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

void tlb_add(Tlb* tlb, int page, int frame) {
    TlbNode* node = tlb_node_create(page, frame);
    TlbNode* prev = tlb_queue_enqueue(tlb -> queue, node, tlb -> policy);
    if (prev != NULL) {
        // full
        tlb_dictionary_remove(tlb -> dictionary, prev -> page);
        tlb_node_destroy(prev);
    }
    tlb_dictionary_add(tlb -> dictionary, node -> page, node);
    if (tlb -> policy == LRU) {
        tlb_queue_put_at_front(tlb -> queue, node);
    }
}

int tlb_get(Tlb* tlb, int page) {
    TlbNode* node = tlb_dictionary_get(tlb -> dictionary, page);
    if (node == NULL) {
        return -1;
    } else if (tlb -> policy == LRU) {
        tlb_queue_put_at_front(tlb -> queue, node);
    }
    return node -> frame;
}

void tlb_remove(Tlb* tlb, int page) {
    TlbNode* node = node = tlb_dictionary_get(tlb -> dictionary, page);
    if (node != NULL) {
        if (node == tlb -> queue -> rear) {
            tlb -> queue -> rear = node -> prev;
        }
        if (node == tlb -> queue -> front) {
            tlb -> queue -> front = node -> next;
        }
        if (node -> next != NULL) {
            node -> next -> prev = node -> prev;
        }
        if (node -> prev != NULL) {
            node -> prev -> next = node -> next;
        }
        tlb -> queue -> current_size--;
        tlb_dictionary_remove(tlb -> dictionary, node -> page);
        tlb_node_destroy(node);
    }
}

void tlb_print(Tlb* tlb) {
    printf("TLB\n");
    printf("i\tpage\tframe\n");
    int counter = 0;
    TlbNode* node = tlb -> queue -> front;
    while (node != NULL) {
        printf("%d\t%d\t%d\n", counter, node -> page, node -> frame);
        counter++;
        node = node -> next;
    }
    for (int i = counter; i < tlb -> queue -> max_size; i++) {
        printf("%d\n", i);
    }
}
