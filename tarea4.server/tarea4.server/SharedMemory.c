#include "SharedMemory.h"

void* shared_memory_alloc(size_t size)
{
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
}

void shared_memory_free(void* shared, size_t size)
{
    munmap(shared, size);
}
