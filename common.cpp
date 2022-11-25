#include "common.h"

const char *shared_memory_name = "shared_memory";
const char *semaphore_a_name = "sem_a";
const char *semaphore_b_name = "sem_b";


void print_error(const std::string &source) {
    std::cerr << source << " error: " << std::strerror(errno) << std::endl;
}

void close_semaphore(sem_t *semaphore) {
    if (sem_close(semaphore) < 0) {
        print_error("sem_close");
    }
}

void close_semaphores(sem_t *sem1, sem_t *sem2) {
    close_semaphore(sem1);
    close_semaphore(sem2);
}

sem_t *create_semaphore(const char *name) {
    std::cout << "creating sem " << semaphore_a_name << std::endl;
    sem_t *sem = sem_open(semaphore_a_name, O_CREAT, 0777, 0);
    if (sem == SEM_FAILED) {
        std::string message = "Cannot open semaphore '";
        message.append(name);
        message.append("': ");
        message.append(std::strerror(errno));

        throw std::runtime_error(message);
    }

    return sem;
}

int open_shared_memory(const char *name, bool is_read_write) {
    int flags;

    if (is_read_write) {
        flags |= O_CREAT | O_RDWR;
    } else {
        flags |= O_RDONLY;
    }

    int shm_handle = shm_open(
            shared_memory_name,
            flags,
            S_IRWXO | S_IRWXG | S_IRWXU
    );

    if (shm_handle < 0) {
        std::string message = "Cannot open shared memory '";
        message.append(name);
        message.append("': ");
        message.append(std::strerror(errno));

        throw std::runtime_error(message);
    }

    return shm_handle;
}

int create_shared_memory(const char *name) {
    int shm_handle = open_shared_memory(name, true);

    if (ftruncate(shm_handle, shared_memory_size) < 0) {
        std::string message = "Cannot ftruncate shared memory '";
        message.append(name);
        message.append("': ");
        message.append(std::strerror(errno));

        throw std::runtime_error(message);
    }

    std::cout << "created shared memory '" << name << "'" << std::endl;

    return shm_handle;
}

void *map_shared_memory(int shm_handle, std::size_t size, bool is_read_write) {
    int protection = is_read_write
               ? PROT_READ | PROT_WRITE
               : PROT_READ;

    void *addr = mmap(nullptr, shared_memory_size, protection, MAP_SHARED, shm_handle, 0);
    if (addr == MAP_FAILED) {
        std::string message = "Cannot map shared memory: ";
        message.append(std::strerror(errno));

        throw std::runtime_error(message);
    }

    return addr;
}

