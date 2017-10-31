#ifndef SharedMemory_h
#define SharedMemory_h

#include <stdlib.h>
#include <sys/mman.h>

void* shared_memory_alloc(size_t size);

void shared_memory_free(void* shared, size_t size);

#endif /* SharedMemory_h */
