#include "../HeaderFiles/client.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <chrono>
#include <unistd.h>
#include <semaphore.h>
#include <random>

void client(int shmid1, sem_t *sem_request, sem_t *sem_response)
{
    char *shm1 = (char *)shmat(shmid1, nullptr, 0);
    if (shm1 == (char *)-1)
    {
        std::cerr << "Error: Failed to attach shared memory!" << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000); // Changed to start from 1 for valid line numbers

    int maxIterations = 5;
    int iterationCount = 0;

    while (iterationCount < maxIterations)
    {
        int lineNumber = dis(gen);
        std::cout << "Generated line number: " << lineNumber << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        std::sprintf(shm1, "%d", lineNumber);
        std::cout << "Line number written to shared memory: " << shm1 << std::endl;

        sem_post(sem_request);
        std::cout << "Request posted to dispatcher." << std::endl;

        sem_wait(sem_response);
        std::cout << "Response received from dispatcher." << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Line: " << shm1 << "\n";
        std::cout << "Time taken: " << elapsed.count() << " seconds.\n";

        shm1[0] = '\0';
        std::cout << "Buffer cleared for next communication." << std::endl;

        sem_post(sem_request);
        std::cout << "Dispatcher notified that the response has been read." << std::endl;

        ++iterationCount;
        usleep(500000); // Sleep for 0.5 seconds to avoid flooding the server
    }

    if (shmdt(shm1) == -1)
    {
        std::cerr << "Error: Failed to detach shared memory!" << std::endl;
    }
}
