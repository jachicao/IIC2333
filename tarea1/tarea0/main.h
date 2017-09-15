#ifndef main_h
#define main_h

typedef struct queue_node_t {
    void* ptr;
    struct queue_node_t* next;
} QueueNode;

typedef struct {
    QueueNode* head;
    QueueNode* tail;
    int size;
} Queue;

enum state { NEW, RUNNING, READY, WAITING, TERMINATED };
typedef struct {
    int pid;
    char* name;
    enum state state;
    int priority;
    int start_time;
    int n;
    Queue* times;
    int time_needed_left;
    int total_time_needed;
    int cpu_usage_counter;
    int blockage_counter;
    int total_turnaround_time;
    int total_response_time;
    int total_waiting_time;
    bool first_time_executed;
} Process;

enum scheduler_type { FCFS, ROUNDROBIN, PRIORITY };

typedef struct {
    enum scheduler_type type;
    int quantum;
    int start_time;
    Process* current_process;
    int total_executions_left;
    
} Scheduler;

#endif /* main_h */
