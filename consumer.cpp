#include <iostream>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstring>

#include "common.h"

int main() {
    sem_t* sem_a = create_semaphore(semaphore_a_name);
    sem_t* sem_b = create_semaphore(semaphore_b_name);

    std::cout << "waiting for shared memory to be initialized" << std::endl;

    sem_wait(sem_b);

    std::cout << "opening shared memory" << std::endl;

    const int shm_handle = open_shared_memory(shared_memory_name);
    void* addr = map_shared_memory(shm_handle, shared_memory_size);

    if (shm_unlink(shared_memory_name) < 0) {
        print_error("shm_unlink");
    }

    std::cout << "shared memory is mapped and unlinked" << std::endl;

    long long sum_a = 0;
    long long sum_b = 0;
    long long sum_c = 0;
    long long sum_d = 0;

    auto* shared_array = static_cast<Record*>(addr);

    for (int i = 0; i < send_iterations; i++) {
        sem_wait(sem_b);

        for (std::size_t j = 0; j < shared_memory_entries; j++) {
            const auto& r = shared_array[j]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
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
