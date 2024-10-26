#include <iostream>
#include <fstream>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstring>
#include "../HeaderFiles/server.h"

void handle_request(RequestServer *request, const std::string &filename) {
    std::cout << "Handling request for line number: " << request->line_number << std::endl;

    if (request->line_number <= 0) { // Add validation
        std::cerr << "Invalid line number requested: " << request->line_number << std::endl;
        return;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    int current_line = 0;
    while (std::getline(file, line)) {
        if (++current_line == request->line_number) {
            strncpy(request->line_content, line.c_str(), sizeof(request->line_content) - 1);
            request->line_content[sizeof(request->line_content) - 1] = '\0'; // Ensure null termination
            std::cout << "Retrieved line " << request->line_number << ": " << request->line_content << std::endl;
            return;
        }
    }

    std::cerr << "Requested line number " << request->line_number << " not found in file." << std::endl;
}

void server(int shm_key, int sem_key) {
    std::cout << "Server starting with SHM_KEY: " << shm_key << " and SEM_KEY: " << sem_key << std::endl;

    int shm_id = shmget(shm_key, sizeof(RequestServer), 0666);
    int sem_id = semget(sem_key, 4, 0666);

    if (shm_id == -1 || sem_id == -1) {
        perror("shmget or semget");
        exit(1);
    }

    std::cout << "Shared memory ID: " << shm_id << std::endl;
    std::cout << "Semaphore ID: " << sem_id << std::endl;

    RequestServer *request = (RequestServer *)shmat(shm_id, nullptr, 0);
    if (request == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    std::cout << "Attached to shared memory." << std::endl;

    struct sembuf sop;

    while (true) {
        sop.sem_num = 1; // Server semaphore
        sop.sem_op = -1; // P operation
        sop.sem_flg = 0;
        std::cout << "Waiting for server semaphore..." << std::endl;
        if (semop(sem_id, &sop, 1) == -1) {
            perror("semop failed (server semaphore)");
            std::cerr << "Failed to perform P operation on server semaphore." << std::endl;
            continue;
        }
        std::cout << "Acquired server semaphore." << std::endl;

        handle_request(request, "random_chars.txt"); // Example file name

        sop.sem_num = 0; // Dispatcher semaphore
        sop.sem_op = 1;  // V operation
        std::cout << "Releasing dispatcher semaphore..." << std::endl;
        if (semop(sem_id, &sop, 1) == -1) {
            perror("semop failed (dispatcher semaphore)");
            std::cerr << "Failed to perform V operation on dispatcher semaphore." << std::endl;
        }
        std::cout << "Dispatcher semaphore released." << std::endl;
    }

    if (shmdt(request) == -1) {
        perror("shmdt");
        exit(1);
    }
    std::cout << "Detached from shared memory." << std::endl;
}
