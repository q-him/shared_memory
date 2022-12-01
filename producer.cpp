#include "common.h"

#include <vector>
#include <random>

void fill(std::vector<Record>& records, std::default_random_engine& rng) {
    std::uniform_int_distribution<int> distribution(Record::left_bound, Record::right_bound);
    for (Record& record : records) {
        record.a = distribution(rng);
        record.b = distribution(rng);
        record.c = distribution(rng);
        record.d = distribution(rng);
    }
}

int main() {
    sem_t* sem_a = create_semaphore(semaphore_a_name);
    sem_t* sem_b = create_semaphore(semaphore_b_name);

    const int shm_handle = create_shared_memory(shared_memory_name);
    void* addr = map_shared_memory(shm_handle, shared_memory_size, true);

    sem_post(sem_b);

    std::cout << "waiting for consumer to start" << std::endl;

    std::random_device random_device;
    std::default_random_engine rng(random_device());
    std::vector<Record> records(shared_memory_entries);

    long long sum_a = 0;
    long long sum_b = 0;
    long long sum_c = 0;
    long long sum_d = 0;

    for (int i = 0; i < send_iterations; i++) {
        fill(records, rng);
        std::memcpy(addr, records.data(), shared_memory_size);

        sem_post(sem_b);

        for (auto r : records) {
            sum_a += r.a;
            sum_b += r.b;
            sum_c += r.c;
            sum_d += r.d;

            std::cout << r << std::endl;
        }

        sem_wait(sem_a);
    }

    std::cout << "sum_a = " << sum_a << std::endl;
    std::cout << "sum_b = " << sum_b << std::endl;
    std::cout << "sum_c = " << sum_c << std::endl;
    std::cout << "sum_d = " << sum_d << std::endl;

    sem_post(sem_b);
    sem_wait(sem_a);

    munmap(addr, shared_memory_size);
    close(shm_handle);
    close_semaphores(sem_a, sem_b);
    if (sem_unlink(semaphore_a_name) < 0) {
        print_error("sem_unlink (a)");
    }
    if (sem_unlink(semaphore_b_name) < 0) {
        print_error("sem_unlink (a)");
    }

    std::cout << "finished successfully" << std::endl;

    return 0;
}
