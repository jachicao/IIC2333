#include "main.h"

// Shared memory
void* shared_memory_alloc(size_t size)
{
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
}

void shared_memory_free(void* shared, size_t size)
{
    munmap(shared, size);
}

bool is_alive(int** array, int m, int n, int i, int j)
{
    return i > -1 && i < m && j > -1 && j < n && array[i][j];
}

int count_alive_around(int** array, int m, int n, int i, int j)
{
    int counter = 0;
    
    int neighbours[8][2] =
    {
        {i - 1	, j - 1	},
        {i			, j - 1	},
        {i + 1	, j - 1	},
        {i + 1	, j			},
        {i + 1	, j + 1	},
        {i			, j + 1	},
        {i - 1	, j + 1	},
        {i - 1	, j			},
    };
    
    for (size_t i = 0; i < 8; i++)
    {
        int* p = neighbours[i];
        int x = p[0];
        int y = p[1];
        if (is_alive(array, m, n, x, y))
        {
            counter++;
        }
    }
    return counter;
}

bool get_new_state(int** array, int m, int n, int i, int j)
{
    int counter = count_alive_around(array, m, n, i, j);
    if (is_alive(array, m, n, i, j))
    {
        if (counter < 2)
        {
            return false;
        }
        else if (counter > 3)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if (counter == 3)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

int** two_d_array_init_int(int m, int n)
{
    int** array = (int**) malloc(m * sizeof(int*));
    for(int i = 0; i < m; i++)
    {
        array[i] = (int*) malloc(n * sizeof(int));
        for (int j = 0; j < n; j++)
        {
            array[i][j] = 0;
        }
    }
    return array;
}

void two_d_array_destroy_int(int** array, int m, int n)
{
    for(int i = 0; i < m; i++)
    {
        free(array[i]);
    }
    free(array);
}

int** two_d_array_copy_int(int** array, int m, int n)
{
    int** new_array = two_d_array_init_int(m, n);
    for(int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            new_array[i][j] = array[i][j];
        }
    }
    return new_array;
}

Worker* worker_create()
{
    Worker* new_worker = (Worker*) malloc(sizeof(Worker));
    new_worker -> array = two_d_array_copy_int(global_master -> array, global_master -> height, global_master -> width);
    new_worker -> pid = -1;
    new_worker -> status = -1;
    return new_worker;
}

void worker_destroy(Worker* worker)
{
    two_d_array_destroy_int(worker -> array, global_master -> height, global_master -> width);
    free(worker);
}

void worker_finished(Worker* worker)
{
    //kill the rest
    for (int i = 0; i < global_master -> workers_length; i++)
    {
        Worker* w = global_master -> workers[i];
        if (w != NULL && worker -> pid != w -> pid)
        {
            kill(w -> pid, SIGTERM);
        }
    }
    master_finished(global_master, worker);
    exit(EXIT_SUCCESS);
}

void* worker_thread_exec(void* arg)
{
    struct thread_args_t* thread_args = arg;
    thread_args -> new_array[thread_args -> i][thread_args -> j] = get_new_state(thread_args -> old_array, global_master -> height, global_master -> width, thread_args -> i, thread_args -> j);
    pthread_exit(0);
    return NULL;
}

void worker_exec(Worker* worker)
{
    pthread_t threads[global_master -> threads];
    for (int iteration = 0; iteration < global_master -> iterations; iteration++)
    {
        int** tmp_array = two_d_array_copy_int(worker -> array, global_master -> height, global_master -> width);
        int index = 0;
        int i = 0;
        int j = 0;
        while (i < global_master -> height && j < global_master -> width)
        {
            ThreadArgs thread_args;
            thread_args.old_array = worker -> array;
            thread_args.new_array = tmp_array;
            thread_args.i = i;
            thread_args.j = j;
            //we assign a cell to each node
            int n = index%global_master -> threads;
            pthread_create(&threads[n], NULL, worker_thread_exec, &thread_args);
            //if thread is busy, we wait until it gets free
            if (pthread_kill(threads[n], 0) == 0)
            {
                pthread_join(threads[n], NULL);
            }
            j++;
            if (j >= global_master -> width)
            {
                j = 0;
                i++;
            }
            index++;
        }
        for(int i = 0; i < global_master -> height; i++)
        {
            for (int j = 0; j < global_master -> width; j++)
            {
                worker -> array[i][j] = tmp_array[i][j];
            }
        }
        two_d_array_destroy_int(tmp_array, global_master -> height, global_master -> width);
        master_worker_notification(global_master, worker);
    }
    worker_finished(worker);
}

Master* master_create(int** array, int iterations, int height, int width, int alive_counter, int threads, int workers_length)
{
    Master* new_master = (Master*) shared_memory_alloc(sizeof(Master));
    new_master -> array = array;
    new_master -> iterations = iterations;
    new_master -> height = height;
    new_master -> width = width;
    new_master -> alive_counter = alive_counter;
    new_master -> threads = threads;
    new_master -> workers_length = workers_length;
    new_master -> finished = false;
    new_master -> workers = (Worker**) malloc(new_master -> workers_length * sizeof(Worker*));
    for (int i = 0; i < new_master -> workers_length; i++)
    {
        new_master -> workers[i] = NULL;
    }
    return new_master;
}

void master_destroy(Master* master)
{
    for (int i = 0; i < master -> workers_length; i++)
    {
        Worker* worker = master -> workers[i];
        if (worker != NULL)
        {
            worker_destroy(worker);
        }
    }
    free(master -> workers);
    
    two_d_array_destroy_int(master -> array, master -> height, master -> width);
    shared_memory_free(master, sizeof(Master));
}

void master_worker_notification(Master* master, Worker* worker)
{
    // worker has notified me the current status
}

void master_finished(Master* master, Worker* worker)
{
    master -> finished = true;
    for(int i = 0; i < master -> height; i++)
    {
        for (int j = 0; j < master -> width; j++)
        {
            master -> array[i][j] = worker -> array[i][j];
        }
    }
    for (int i = 0; i < global_master -> height; i++)
    {
        for (int j = 0; j < global_master -> width; j++)
        {
            printf("%d", global_master -> array[i][j]);
            if (j < global_master -> width - 1)
            {
                printf(" ");
            }
        }
        printf("\n");
    }
    master_destroy(global_master);
}

int main(int argc, const char * argv[])
{
    if (argc < 2)
    {
        printf("Falta input. Ej: input.txt\n");
        return EXIT_SUCCESS;
    }
    FILE* file = fopen(argv[1], "r");
    if (file == NULL)
    {
        printf("Failed to read file");
        return EXIT_SUCCESS;
    }
    
    int t, f, c, v, n;
    fscanf(file, "%d %d %d %d %d", &t, &f, &c, &v, &n);
    int** array = two_d_array_init_int(f, c);
    for (int i = 0; i < v; i++)
    {
        int x;
        int y;
        fscanf(file, "%d %d", &x, &y);
        array[x][y] = 1;
    }
    fclose(file);
    size_t cpu_cores = sysconf(_SC_NPROCESSORS_ONLN);
    global_master = master_create(array, t, f, c, v, n, (int) cpu_cores);
    for (int i = 0; i < global_master -> workers_length && !global_master -> finished; i++)
    {
        Worker* worker = worker_create();
        global_master -> workers[i] = worker;
        pid_t pid = fork();
        if (pid == 0)
        {
            worker_exec(worker);
        }
        else
        {
            worker -> pid = pid;
            waitpid(worker -> pid, &(worker -> status), 0);
        }
    }
    return 0;
}
