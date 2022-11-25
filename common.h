#ifndef SHARED_MEMORY_COMMON_H
#define SHARED_MEMORY_COMMON_H

#include <iostream>
#include <semaphore.h>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <exception>

struct Record {
    int a;
    int b;
    int c;
    int d;
};

constexpr std::size_t shared_memory_entries = 10000;
constexpr std::size_t shared_memory_size = sizeof(Record) * shared_memory_entries;

extern const char *shared_memory_name;
extern const char *semaphore_a_name;
extern const char *semaphore_b_name;

void print_error(const std::string &source);

sem_t *create_semaphore(const char *name);

void close_semaphores(sem_t *sem1, sem_t *sem2);

int open_shared_memory(const char* name, bool is_read_write = false);

int create_shared_memory(const char* name);

void *map_shared_memory(int shm_handle, std::size_t size, bool is_read_write = false);

#endif //SHARED_MEMORY_COMMON_H