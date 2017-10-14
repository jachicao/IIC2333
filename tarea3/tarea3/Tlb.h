#ifndef Tlb_h
#define Tlb_h

#include <stdlib.h>
#include <stdbool.h>
#include "Constants.h"
#include "Enums.h"
#include "PageTable.h"
#include "Statistics.h"

extern Statistics* global_statistics;

typedef struct TlbNode
{
    struct TlbNode* prev;
    struct TlbNode* next;
    int page;
    int frame;
} TlbNode;

TlbNode* tlb_node_create(int page, int frame);
void tlb_node_destroy(TlbNode* node);

typedef struct TlbQueue
{
    int current_size;
    int max_size;
    TlbNode* front;
    TlbNode* rear;
} TlbQueue;

TlbQueue* tlb_queue_create(int max_size);
void tlb_queue_destroy(TlbQueue* queue);
bool tlb_queue_is_empty(TlbQueue* queue);
bool tlb_queue_is_full(TlbQueue* queue);
TlbNode* tlb_queue_enqueue(TlbQueue* queue, TlbNode* node, enum policy_type policy);
void tlb_queue_put_at_front(TlbQueue* queue, TlbNode* node);
TlbNode* tlb_queue_remove_front(TlbQueue* queue);
TlbNode* tlb_queue_remove_rear(TlbQueue* queue);

typedef struct TlbDictionary
{
    int size;
    TlbNode** array;
} TlbDictionary;

TlbDictionary* tlb_dictionary_create(int size);
void tlb_dictionary_destroy(TlbDictionary* dictionary);
bool tlb_dictionary_contains(TlbDictionary* dictionary, int key);
TlbNode* tlb_dictionary_add(TlbDictionary* dictionary, int key, TlbNode* node);
TlbNode* tlb_dictionary_remove(TlbDictionary* dictionary, int key);
TlbNode* tlb_dictionary_get(TlbDictionary* dictionary, int key);


typedef struct Tlb
{
    enum policy_type policy;
    TlbQueue* queue;
    TlbDictionary* dictionary;
} Tlb;

Tlb* tlb_create(enum policy_type policy);
void tlb_destroy(Tlb* tlb);
void tlb_add(Tlb* tlb, int page, int frame);
int tlb_get(Tlb* tlb, int page);
void tlb_remove(Tlb* tlb, int page);
void tlb_print(Tlb* tlb);

#endif /* Tlb_h */

