#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int get_int_from_ptr(void* ptr) {
    return *(int*) ptr;
}

char* get_string_from_ptr(void* ptr) {
    return *(char**) ptr;
}

void print_int_from_ptr(void* ptr) {
    printf("%i", get_int_from_ptr(ptr));
}

void print_string_from_ptr(void* ptr) {
    printf("%s", get_string_from_ptr(ptr));
}

typedef struct queue_node_t {
    void* ptr;
    struct queue_node_t* previous;
} QueueNode;

QueueNode* queue_node_init(void* ptr, size_t value_size) {
    QueueNode* new_node = (QueueNode*) malloc(sizeof(QueueNode));
    new_node -> ptr = malloc(value_size);
    memcpy(new_node -> ptr, ptr, value_size);
    new_node -> previous = NULL;
    return new_node;
}

void queue_node_destroy(QueueNode* node) {
    free(node -> ptr);
    free(node);
}

int queue_node_get_int(QueueNode* node) {
    return get_int_from_ptr(node -> ptr);
}

char* queue_node_get_string(QueueNode* node) {
    return get_string_from_ptr(node -> ptr);
}

typedef struct {
    QueueNode* head;
    QueueNode* tail;
    int size;
} Queue;

Queue* queue_init() {
    Queue* new_queue = (Queue*) malloc(sizeof(Queue));
    
    new_queue -> head = NULL;
    new_queue -> tail = NULL;
    new_queue -> size = 0;
    
    return new_queue;
}

void queue_push(Queue* queue, void* ptr, size_t value_size) {
    QueueNode* new_node = queue_node_init(ptr, value_size);
    if (queue -> size == 0) {
        queue -> head = new_node;
    } else {
        queue -> tail -> previous = new_node;
    }
    queue -> tail = new_node;
    queue -> size++;
}

QueueNode* queue_pop(Queue* queue) {
    QueueNode* node = queue -> head;
    if (node != NULL) {
        queue -> head = node -> previous;
        queue -> size--;
        return node;
    }
    return NULL;
}

void queue_destroy(Queue* queue) {
    QueueNode* node = queue -> head;
    while (node != NULL) {
        
        QueueNode* previous = node -> previous;
        queue_node_destroy(node);
        node = previous;
    }
    free(queue);
}

void queue_print(Queue* queue, void (*print_function_prt) (void*)) {
    QueueNode* node = queue -> head;
    if (node != NULL) {
        printf("[");
        while (node != NULL) {
            (*print_function_prt)(node -> ptr);
            node = node -> previous;
            if (node != NULL) {
                printf(", ");
            }
        }
        printf("]\n");
    }
}

typedef struct linked_list_node_t {
    void* ptr;
    struct linked_list_node_t* next;
} LinkedListNode;


LinkedListNode* linked_list_node_init(void* ptr, size_t value_size) {
    LinkedListNode* new_node = (LinkedListNode*) malloc(sizeof(LinkedListNode));
    new_node -> ptr = malloc(value_size);
    memcpy(new_node -> ptr, ptr, value_size);
    new_node -> next = NULL;
    return new_node;
}

void linked_list_node_destroy(LinkedListNode* node) {
    free(node -> ptr);
    free(node);
}

int linked_list_node_get_int(LinkedListNode* node) {
    return get_int_from_ptr(node -> ptr);
}

char* linked_list_node_get_string(LinkedListNode* node) {
    return get_string_from_ptr(node -> ptr);
}

typedef struct {
    LinkedListNode* head;
    LinkedListNode* tail;
    int size;
} LinkedList;

LinkedList* linked_list_init() {
    LinkedList* new_linked_list = (LinkedList*) malloc(sizeof(LinkedList));
    new_linked_list -> head = NULL;
    new_linked_list -> size = 0;
    return new_linked_list;
}

void linked_list_add(LinkedList* linked_list, void* ptr, size_t value_size) {
    LinkedListNode* node = linked_list_node_init(ptr, value_size);
    if (linked_list -> size == 0) {
        linked_list -> head = node;
    } else {
        linked_list -> tail -> next = node;
    }
    linked_list -> tail = node;
    linked_list -> size++;
}

void* linked_list_get(LinkedList* linked_list, int index) {
    int i = 0;
    LinkedListNode* node = linked_list -> head;
    while (node != NULL) {
        if (i == index) {
            return node -> ptr;
        }
    }
    return NULL;
}

void linked_list_remove(LinkedList* linked_list, int index) {
    int i = 0;
    LinkedListNode* previous = NULL;
    LinkedListNode* current = linked_list -> head;
    while (current != NULL) {
        if (i == index) {
            linked_list -> size--;
            if (previous != NULL) {
                previous -> next = current -> next;
            } else {
                linked_list -> head = current -> next;
            }
            linked_list_node_destroy(current);
            break;
        }
        previous = current;
        current = previous -> next;
        i++;
    }
}

void linked_list_print(LinkedList* linked_list, void (*print_function_prt) (void*)) {
    LinkedListNode* node = linked_list -> head;
    if (node != NULL) {
        printf("[");
        while (node != NULL) {
            (*print_function_prt)(node -> ptr);
            node = node -> next;
            if (node != NULL) {
                printf(", ");
            }
        }
        printf("]\n");
    }
}


void linked_list_destroy(LinkedList* linked_list) {
    LinkedListNode* node = linked_list -> head;
    while (node != NULL) {
        
        LinkedListNode* next = node -> next;
        linked_list_node_destroy(node);
        node = next;
    }
    free(linked_list);
}

enum state { RUNNING, READY, WAITING };

typedef struct {
    int pid;
    char* name;
    enum state state;
} Process;

Process* process_init(int pid, char* name) {
    Process* new_process = (Process*) malloc(sizeof(Process));
    new_process -> pid = pid;
    new_process -> name = name;
    new_process -> state = READY;
    
    return new_process;
}


void process_destroy(Process* process) {
    free(process);
}

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    int value = 5;
    int value2 = 6;
    LinkedList* linked_list = linked_list_init();
    
    linked_list_add(linked_list, &value, sizeof(value));
    linked_list_add(linked_list, &value2, sizeof(value2));
    linked_list_print(linked_list, print_int_from_ptr);
    linked_list_remove(linked_list, 0);
    linked_list_remove(linked_list, 0);
    linked_list_print(linked_list, print_int_from_ptr);
    linked_list_destroy(linked_list);
    return 0;
}
