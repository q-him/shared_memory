#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstring>

#include "common.h"

int main() {
    sem_t* sem_a = create_semaphore(semaphore_a_name);
    sem_t* sem_b = create_semaphore(semaphore_b_name);

    sem_post(sem_a);
    sem_wait(sem_b);

    int shm_handle = open_shared_memory(shared_memory_name);
    void* addr = map_shared_memory(shm_handle, shared_memory_size);

    if (shm_unlink(shared_memory_name) < 0) {
        print_error("shm_unlink");
    }

    sem_wait(sem_b);

    long long sum_a;
    long long sum_b;
    long long sum_c;
    long long sum_d;

    auto* shared_array = (Record *)addr;

    for (int i = 0; i < 100; i++) {
        sem_wait(sem_b);
        for (std::size_t j = 0; j < shared_memory_entries; j++) {
            Record r = shared_array[j];
            sum_a += r.a;
            sum_b += r.b;
            sum_c += r.c;
            sum_d += r.d;
        }
        sem_post(sem_a);
    }

    sem_post(sem_a);

    std::cout << "sum_a = " << sum_a << std::endl;
    std::cout << "sum_b = " << sum_b << std::endl;
    std::cout << "sum_c = " << sum_c << std::endl;
    std::cout << "sum_d = " << sum_d << std::endl;

    munmap(addr, shared_memory_size);
    close(shm_handle);
    close_semaphores(sem_a, sem_b);

    return 0;
}
