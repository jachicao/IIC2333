#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <math.h>
#include <limits.h>
#include "main.h"


Queue* all_processes = NULL;
Queue* queue_pids = NULL;
Scheduler* current_scheduler = NULL;
int tick_counter = 0;
int program_start_time = 0;

int get_current_period() {
    /*
    time_t tmp_time;
    time(&tmp_time);
    return (int) tmp_time;
     */
    return tick_counter;
}

void* new_value(void* ptr, size_t value_size) {
    void* new_ptr = malloc(value_size);
    memcpy(new_ptr, ptr, value_size);
    return new_ptr;
}

char* read_file(char* filepath)
{
    char* buffer = NULL;
    FILE* handler = fopen(filepath, "r");
    if (handler != NULL) {
        fseek(handler, 0, SEEK_END);
        long pos = ftell(handler);
        fseek(handler, 0, SEEK_SET);
        
        buffer = (char*) malloc(1 + pos);
        fread(buffer, pos, 1, handler);
        fclose(handler);
        buffer[pos] = '\0'; // just for valgrind xd
    } else {
        printf("Failed to read %s\n", filepath);
        exit(EXIT_FAILURE);
    }
    return buffer;
}

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

void** array_init(int* ptr_array_size) {
    *ptr_array_size = 0;
    return (void**) malloc((*ptr_array_size + 1) * sizeof(void*));
}

void array_insert(void*** ptr_array, int* ptr_array_size, void* ptr_value, size_t value_size) {
    int n = *ptr_array_size;
    void** tmp = (void**) realloc(*ptr_array, (n + 1) * sizeof(void*));
    if (tmp != NULL) {
        *ptr_array = tmp;
        (*ptr_array)[n] = malloc(value_size);
        if ((*ptr_array)[n] != NULL) {
            memcpy((*ptr_array)[n], ptr_value, value_size);
        }
    }
    (*ptr_array_size)++;
}

void array_destroy(void*** ptr_array, int* ptr_array_size) {
    for (size_t i = 0; i < *ptr_array_size; i++) {
        free((*ptr_array)[i]);
    }
    free(*ptr_array);
}



void** string_split(char** str, char* separator, int* array_size) {
    
    void** array = array_init(array_size);
    
    if (array != NULL) {
        char* token = strtok(*str, separator);
        while (token != NULL)
        {
            array_insert(&array, array_size, &token, sizeof(token));
            token = strtok(NULL, separator);
        }
    }
    return array;
}

QueueNode* queue_node_init(void* ptr, size_t value_size) {
    QueueNode* new_node = (QueueNode*) malloc(sizeof(QueueNode));
    new_node -> ptr = malloc(value_size);
    memcpy(new_node -> ptr, ptr, value_size);
    new_node -> next = NULL;
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

Queue* queue_init() {
    Queue* new_queue = (Queue*) malloc(sizeof(Queue));
    new_queue -> head = NULL;
    new_queue -> size = 0;
    return new_queue;
}

void queue_add(Queue* queue, void* ptr, size_t value_size) {
    QueueNode* node = queue_node_init(ptr, value_size);
    if (queue -> size == 0) {
        queue -> head = node;
    } else {
        queue -> tail -> next = node;
    }
    queue -> tail = node;
    queue -> size++;
}

QueueNode* queue_pop(Queue* queue) {
    QueueNode* node = queue -> head;
    if (node != NULL) {
        queue -> head = node -> next;
        queue -> size--;
        return node;
    }
    return NULL;
}

void* queue_get(Queue* queue, int index) {
    int i = 0;
    QueueNode* node = queue -> head;
    while (node != NULL) {
        if (i == index) {
            return node -> ptr;
        }
        node = node -> next;
        i++;
    }
    return NULL;
}

void queue_remove_at(Queue* queue, int index) {
    int i = 0;
    QueueNode* previous = NULL;
    QueueNode* current = queue -> head;
    while (current != NULL) {
        if (i == index) {
            queue -> size--;
            if (previous != NULL) {
                previous -> next = current -> next;
            } else {
                queue -> head = current -> next;
            }
            queue_node_destroy(current);
            break;
        }
        previous = current;
        current = previous -> next;
        i++;
    }
}

void queue_print(Queue* queue, void (*print_function_prt) (void*)) {
    QueueNode* node = queue -> head;
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


void queue_destroy(Queue* queue) {
    QueueNode* node = queue -> head;
    while (node != NULL) {
        QueueNode* next = node -> next;
        queue_node_destroy(node);
        node = next;
    }
    free(queue);
}

Process* process_init(int pid, char* string) {
    Process* new_process = (Process*) malloc(sizeof(Process));
    int split_size;
    void** split = string_split(&string, " ", &split_size);
    new_process -> pid = pid;
    new_process -> name = strdup(get_string_from_ptr(split[0]));
    new_process -> state = NEW;
    new_process -> priority = atoi(get_string_from_ptr(split[1]));
    new_process -> start_time = atoi(get_string_from_ptr(split[2]));
    new_process -> n = atoi(get_string_from_ptr(split[3]));
    new_process -> times = queue_init();
    new_process -> total_time_needed = 0;
    new_process -> time_needed_left = 0;
    new_process -> cpu_usage_counter = 0;
    new_process -> blockage_counter = 0;
    new_process -> total_turnaround_time = 0;
    new_process -> total_response_time = 0;
    new_process -> total_waiting_time = 0;
    new_process -> first_time_executed = false;
    for (int i = 0; i < new_process -> n * 2 - 1; i++) {
        int value = atoi(get_string_from_ptr(split[4 + i]));
        queue_add(new_process -> times, &value, sizeof(value));
    }
    array_destroy(&split, &split_size);
    
    printf("Proceso %s ha sido creado\n", new_process -> name);
    
    return new_process;
}

char* process_get_string_state(enum state state) {
    switch (state) {
        case NEW:
            return "NEW";
        case RUNNING:
            return "RUNNING";
        case READY:
            return "READY";
        case WAITING:
            return "WAITING";
        case TERMINATED:
            return "TERMINATED";
        default:
            break;
    }
    return NULL;
}

void process_change_state(Process* process, enum state new_state) {
    printf("Proceso %s ha cambiado de estado %s a %s\n", process -> name, process_get_string_state(process -> state), process_get_string_state(new_state));
    enum state current_state = process -> state;
    if (current_state == READY && new_state == RUNNING) {
        process -> cpu_usage_counter++;
        process -> first_time_executed = true;
    }
    if (current_state == RUNNING && new_state == WAITING) {
        process -> blockage_counter++;
    }
    int pid = process -> pid;
    switch (new_state) {
        case READY:
            queue_add(queue_pids, &pid, sizeof(pid));
            break;
        default:
            break;
    }
    process -> state = new_state;
}

/*
void process_start_new_state(Process* process, int time, enum state state) {
    process -> time_needed_left = time;
    process -> total_time_needed = time;
    process_change_state(process, state);
}
*/

void process_run(Process* process) {
    process -> time_needed_left --;
    printf("Proceso %s ha sido ejecutado %i periodo(s), faltando %i periodo(s)\n", process -> name, process -> total_time_needed - process -> time_needed_left, process -> time_needed_left);
}

void process_wait(Process* process) {
    process -> time_needed_left --;
    //printf("Proceso %s ha estado bloqueado por %i periodo(s), faltando %i periodo(s)\n", process -> name, process -> total_time_needed - process -> time_needed_left, process -> time_needed_left);
}

void process_destroy(Process* process) {
    printf("Proceso %s ha sido destruido\n", process -> name);
    queue_destroy(process -> times);
    free(process -> name);
    free(process);
}

Process* get_process_from_pid(int pid) {
    QueueNode* node = all_processes -> head;
    while (node != NULL) {
        Process* process = *(Process**) node -> ptr;
        if (process -> pid == pid) {
            return process;
        }
        node = node -> next;
    }
    return NULL;
}

Scheduler* scheduler_init(enum scheduler_type type, int quantum) {
    Scheduler* new_scheduler = (Scheduler*) malloc(sizeof(Scheduler));
    new_scheduler -> type = type;
    new_scheduler -> quantum = quantum;
    new_scheduler -> current_process = NULL;
    new_scheduler -> start_time = get_current_period();
    new_scheduler -> total_executions_left = 0;
    
    return new_scheduler;
}

void scheduler_destroy(Scheduler* scheduler) {
    free(scheduler);
}

int scheduler_get_priority(Scheduler* scheduler, Process* process) {
    int priority = process -> priority;
    int quantum = scheduler -> quantum;
    switch (scheduler -> type) {
        case PRIORITY:
            return priority;
        case ROUNDROBIN:
            return priority * quantum + pow(-1, round(((double) priority) / ((double) quantum))) * priority;
        default:
            break;
    }
    return 0;
}

int scheduler_get_max_run_time(Scheduler* scheduler, Process* process) {
    int priority = scheduler_get_priority(scheduler, process);
    switch (scheduler -> type) {
        case ROUNDROBIN:
            return ceil(((double) priority) / ((double)64));
        default:
            break;
    }
    return INT_MAX;
}

void scheduler_select_process(Scheduler* scheduler, Process* process) {
    int run_time = process -> time_needed_left;
    if (run_time <= 0) {
        QueueNode* node = queue_pop(process -> times);
        if (node != NULL) {
            void* ptr = node -> ptr;
            if (ptr != NULL) {
                run_time = get_int_from_ptr(ptr);
                process -> time_needed_left = run_time;
                process -> total_time_needed = run_time;
            }
            queue_node_destroy(node);
        }
    }
    int max_time = scheduler_get_max_run_time(scheduler, process);
    int scheduler_time = run_time > max_time ? max_time : run_time;
    printf("Scheduler ha escogido al proceso %s para ser ejecutado durante %i periodo(s)\n", process -> name, scheduler_time);
    scheduler -> total_executions_left = scheduler_time;
    scheduler -> current_process = process;
    process_change_state(process, RUNNING);
}

void scheduler_remove_current_process(Scheduler* scheduler) {
    Process* process = scheduler -> current_process;
    if (process != NULL) {
        if (process -> time_needed_left <= 0) {
            QueueNode* node = queue_pop(process -> times);
            if (node != NULL) {
                void* ptr = node -> ptr;
                if (ptr != NULL) {
                    int run_time = get_int_from_ptr(ptr);
                    process -> time_needed_left = run_time;
                    process -> total_time_needed = run_time;
                    process_change_state(process, WAITING);
                }
                queue_node_destroy(node);
            } else {
                process_change_state(process, TERMINATED);
            }
        } else {
            process_change_state(process, READY);
        }
        scheduler -> current_process = NULL;
    }
}

void scheduler_run(Scheduler* scheduler) {
    int current_time = get_current_period();
    QueueNode* node = all_processes -> head;
    while (node != NULL) {
        Process* process = *(Process**) node -> ptr;
        enum state process_state = process -> state;
        switch (process_state) {
            case NEW:
                if (process -> start_time <= current_time - program_start_time) {
                    process_change_state(process, READY);
                }
                break;
            case WAITING:
                if (process -> time_needed_left <= 0) {
                    process_change_state(process, READY);
                }
                break;
            default:
                break;
        }
        node = node -> next;
    }
    if (scheduler -> current_process != NULL) {
        Process* process = scheduler -> current_process;
        enum state process_state = process -> state;
        switch (process_state) {
            case RUNNING:
                if (scheduler -> total_executions_left <= 0) {
                    scheduler_remove_current_process(scheduler);
                }
                break;
            default:
                break;
        }
    }
    if (scheduler -> current_process == NULL) {
        int best_pid = -1;
        int best_priority = -1;
        int index = 0;
        QueueNode* node = queue_pids -> head;
        switch (scheduler -> type) {
            case FCFS:
                while (node != NULL) {
                    int pid = get_int_from_ptr(node -> ptr);
                    Process* process = get_process_from_pid(pid);
                    if (process -> state == READY) {
                        best_pid = pid;
                        break;
                    }
                    node = node -> next;
                    index++;
                }
                break;
            case PRIORITY:
                while (node != NULL) {
                    int pid = get_int_from_ptr(node -> ptr);
                    Process* process = get_process_from_pid(pid);
                    if (process -> state == READY) {
                        int priority = scheduler_get_priority(scheduler, process);
                        if (best_priority < priority) {
                            best_priority = priority;
                            best_pid = pid;
                        }
                        break;
                    }
                    node = node -> next;
                    index++;
                }
                break;
            case ROUNDROBIN:
                while (node != NULL) {
                    int pid = get_int_from_ptr(node -> ptr);
                    Process* process = get_process_from_pid(pid);
                    if (process -> state == READY) {
                        int priority = scheduler_get_priority(scheduler, process);
                        if (best_priority < priority) {
                            best_priority = priority;
                            best_pid = pid;
                        }
                        break;
                    }
                    node = node -> next;
                    index++;
                }
            default:
                break;
        }
        if (best_pid > -1) {
            queue_remove_at(queue_pids, index);
            Process* best_process = get_process_from_pid(best_pid);
            scheduler_select_process(current_scheduler, best_process);
        }
    }
    
    node = all_processes -> head;
    while (node != NULL) {
        Process* process = *(Process**) node -> ptr;
        enum state process_state = process -> state;
        switch (process_state) {
            case WAITING:
                process_wait(process);
                break;
            default:
                break;
        }
        node = node -> next;
    }
    if (scheduler -> current_process != NULL) {
        Process* process = scheduler -> current_process;
        enum state process_state = process -> state;
        switch (process_state) {
            case RUNNING:
                if (scheduler -> total_executions_left > 0) {
                    process_run(process);
                    scheduler -> total_executions_left--;
                }
                break;
            default:
                break;
        }
    }
    node = all_processes -> head;
    while (node != NULL) {
        Process* process = *(Process**) node -> ptr;
        enum state process_state = process -> state;
        if (process_state == READY) {
            process -> total_waiting_time++;
        }
        if (process_state == WAITING || process_state == READY || process_state == RUNNING) {
            process -> total_turnaround_time++;
        }
        if (process_state == READY && process -> first_time_executed == false) {
            process -> total_response_time++;
        }
        node = node -> next;
    }
    //printf("\n");
}

void program_end(int s) {
    printf("Simulacion terminada\n");
    
    int terminated_counter = 0;
    QueueNode* node = all_processes -> head;
    while (node != NULL) {
        Process* process = *(Process**) node -> ptr;
        if (process -> state == TERMINATED) {
            terminated_counter++;
        }
        node = node -> next;
    }
    
    printf("- - - - - - - - - - - -\n");
    
    printf("%i procesos terminaron su ejecucion\n", terminated_counter);
    
    if (current_scheduler != NULL) {
        printf("Simulacion duro %i periodo(s)\n", get_current_period() - current_scheduler -> start_time);
        
        scheduler_destroy(current_scheduler);
    }
    
    printf("- - - - - - - - - - - -\n");
    node = all_processes -> head;
    while (node != NULL) {
        Process* process = *(Process**) node -> ptr;
        printf("- %s: \n", process -> name);
        printf("\tNumero de veces que fue elegido para usar la CPU: %i\n", process -> cpu_usage_counter);
        printf("\tNumero de veces que se bloqueo: %i\n", process -> blockage_counter);
        printf("\tTurnaround time: %i\n", process -> total_turnaround_time);
        printf("\tResponse time: %i\n", process -> total_response_time);
        printf("\tWaiting time: %i\n", process -> total_waiting_time);
        printf("\n");
        node = node -> next;
    }
    printf("- - - - - - - - - - - -\n");
    
    if (queue_pids != NULL) {
        queue_destroy(queue_pids);
    }
    
    if (all_processes != NULL) {
        QueueNode* node = all_processes -> head;
        while (node != NULL) {
            Process* process = *(Process**) node -> ptr;
            process_destroy(process);
            node = node -> next;
        }
        queue_destroy(all_processes);
    }
    exit(EXIT_SUCCESS);
}


int main(int argc, const char * argv[]) {
    
    if (argc < 3) {
        printf("Faltan parametros\n");
        return EXIT_FAILURE;
    }
    
    enum scheduler_type scheduler_type;
    
    if (strcmp(argv[1], "fcfs") == 0) {
        scheduler_type = FCFS;
    } else if (strcmp(argv[1], "roundrobin") == 0) {
        scheduler_type = ROUNDROBIN;
    } else if (strcmp(argv[1], "priority") == 0) {
        scheduler_type = PRIORITY;
    } else {
        printf("Scheduler desconocido\n");
        return EXIT_FAILURE;
    }
    int quantum = 3;
    if (scheduler_type == ROUNDROBIN) {
        if (argc >= 4) {
            char* number = strdup(argv[3]);
            quantum = atoi(number);
            free(number);
        }
    }
    
    current_scheduler = scheduler_init(scheduler_type, quantum);
    
    char* file_path = strdup(argv[2]);
    char* lines = read_file(file_path);
    free(file_path);
    
    int split_lines_size;
    void** split_lines = string_split(&lines, "\n", &split_lines_size);
    
    all_processes = queue_init();
    queue_pids = queue_init();
    for (int i = 0; i < split_lines_size; i++) {
        char* line = get_string_from_ptr(split_lines[i]);
        Process* process = process_init(i, line);
        queue_add(all_processes, &process, sizeof(process));
    }
    
    array_destroy(&split_lines, &split_lines_size);
    free(lines);
    
    struct sigaction sig_handler;
    sig_handler.sa_handler = program_end;
    sigemptyset(&sig_handler.sa_mask);
    sig_handler.sa_flags = 0;
    sigaction(SIGINT, &sig_handler, NULL);
    
    program_start_time = get_current_period();
    int previous_time = get_current_period();
    while (1) {
        int current_time = get_current_period();
        if (previous_time < current_time) {
            bool all_processes_have_terminated = true;
            QueueNode* node = all_processes -> head;
            while (node != NULL) {
                Process* process = *(Process**) node -> ptr;
                if (process -> state != TERMINATED) {
                    all_processes_have_terminated = false;
                }
                node = node -> next;
            }
            if (all_processes_have_terminated) {
                break;
            }
            
            scheduler_run(current_scheduler);
            previous_time = current_time;
        }
        
        tick_counter++;
    }
    
    program_end(0);
    
    return EXIT_SUCCESS;
}
