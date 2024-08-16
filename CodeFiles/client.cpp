#include "../HeaderFiles/client.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <chrono>
#include <unistd.h>
#include <semaphore.h>

void client(int shmid1, sem_t *sem_request, sem_t *sem_response)
{
    // Attach shared memory
    char *shm1 = (char *)shmat(shmid1, nullptr, 0);
    if (shm1 == (char *)-1)
    {
        std::cerr << "Error: Failed to attach shared memory!" << std::endl;
        return;
    }

    while (true)
    {
        int lineNumber;
        std::cout << "Enter the line number: ";
        std::cin >> lineNumber;

        auto start = std::chrono::high_resolution_clock::now();

        // Wait for the dispatcher to be ready for a new request
        sem_wait(sem_response);

        // Copy the line number to shared memory
        std::sprintf(shm1, "%d", lineNumber);

        // Signal the dispatcher that a new request is available
        sem_post(sem_request);

        // Wait for the response from the dispatcher
        sem_wait(sem_response);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        // Print the result
        std::cout << "Line: " << shm1 << "\n";
        std::cout << "Time taken: " << elapsed.count() << " seconds.\n";

        // Clear the buffer for the next communication
        shm1[0] = '\0';

        // Signal the dispatcher that the response has been read
        sem_post(sem_request);
    }

    // Detach from shared memory
    if (shmdt(shm1) == -1)
    {
        std::cerr << "Error: Failed to detach shared memory!" << std::endl;
    }
}
