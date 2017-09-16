#ifndef main_h
#define main_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>

typedef struct worker_t
{
    int** array;
    pid_t pid;
    int status;
} Worker;

typedef struct master_t
{
    int** array;
    struct worker_t** workers;
    int workers_length;
    int iterations;
    int height;
    int width;
    int alive_counter;
    int threads;
    bool finished;
} Master;

typedef struct thread_args_t
{
    int** old_array;
    int** new_array;
    int i;
    int j;
} ThreadArgs;

void master_worker_notification(Master* master, Worker* worker);
void master_finished(Master* master, Worker* worker);

//Shared memory
static Master* global_master = NULL;

#endif /* main_h */
