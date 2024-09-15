#include <iostream>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ctime>
#include <cstdlib>
#include "../HeaderFiles/client.h"

void client(int shm_key, int sem_key)
{
    std::cout << "Client process started with SHM_KEY: " << shm_key << " and SEM_KEY: " << sem_key << std::endl;

    int shm_id = shmget(shm_key, sizeof(RequestClient), 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        std::cerr << "Failed to get shared memory ID. SHM_KEY: " << shm_key << std::endl;
        exit(1);
    }
    std::cout << "Shared memory ID: " << shm_id << std::endl;

    int sem_id = semget(sem_key, 4, 0666);
    if (sem_id == -1) {
        perror("semget failed");
        std::cerr << "Failed to get semaphore ID. SEM_KEY: " << sem_key << std::endl;
        exit(1);
    }
    std::cout << "Semaphore ID: " << sem_id << std::endl;

    RequestClient *request = (RequestClient *)shmat(shm_id, nullptr, 0);
    if (request == (void *)-1) {
        perror("shmat failed");
        std::cerr << "Failed to attach shared memory. SHM_ID: " << shm_id << std::endl;
        exit(1);
    }
    std::cout << "Attached to shared memory successfully." << std::endl;

    srand(time(nullptr) ^ getpid()); // Ensure unique seed for each process
    int line_number = rand() % 100 + 1;
    if (line_number <= 0) {
        std::cerr << "Generated invalid line number: " << line_number << std::endl;
        shmdt(request);
        exit(1);
    }
    std::cout << "Generated random line number: " << line_number << std::endl;

    struct sembuf sop;

    sop.sem_num = 0; // Dispatcher semaphore
    sop.sem_op = -1; // P operation
    sop.sem_flg = 0;
    std::cout << "Waiting for dispatcher semaphore..." << std::endl;
    if (semop(sem_id, &sop, 1) == -1) {
        perror("semop failed (dispatcher semaphore)");
        std::cerr << "Failed to perform P operation on dispatcher semaphore." << std::endl;
        shmdt(request);
        exit(1);
    }
    std::cout << "Acquired dispatcher semaphore." << std::endl;

    request->line_number = line_number;
    request->client_id = getpid();
    std::cout << "Set request: line_number = " << line_number << ", client_id = " << getpid() << std::endl;

    sop.sem_num = 1; // Server semaphore
    sop.sem_op = 1;  // V operation
    std::cout << "Releasing server semaphore..." << std::endl;
    if (semop(sem_id, &sop, 1) == -1) {
        perror("semop failed (server semaphore)");
        std::cerr << "Failed to perform V operation on server semaphore." << std::endl;
        shmdt(request);
        exit(1);
    }
    std::cout << "Server semaphore released." << std::endl;

    sop.sem_num = 2; // Dispatcher semaphore
    sop.sem_op = -1; // P operation
    std::cout << "Waiting for dispatcher semaphore to complete..." << std::endl;
    if (semop(sem_id, &sop, 1) == -1) {
        perror("semop failed (dispatcher semaphore)");
        std::cerr << "Failed to perform P operation on dispatcher semaphore." << std::endl;
        shmdt(request);
        exit(1);
    }
    std::cout << "Received response from dispatcher." << std::endl;

    std::cout << "Client " << getpid() << " requested line " << line_number << ": " << request->line_content << std::endl;

    if (shmdt(request) == -1) {
        perror("shmdt failed");
        std::cerr << "Failed to detach from shared memory. SHM_ID: " << shm_id << std::endl;
        exit(1);
    }
    std::cout << "Detached from shared memory successfully." << std::endl;
}
