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
        std::cout << "Entered line number: " << lineNumber << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        // Write the line number to shared memory
        std::sprintf(shm1, "%d", lineNumber);
        std::cout << "Line number written to shared memory: " << shm1 << std::endl;

        // Notify the dispatcher that a request is available
        sem_post(sem_request);
        std::cout << "Request posted to dispatcher." << std::endl;

        // Wait for the response from the dispatcher
        std::cout << "Waiting for dispatcher response..." << std::endl;
        sem_wait(sem_response);
        std::cout << "Response received from dispatcher." << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        // Print the result
        std::cout << "Line: " << shm1 << "\n";
        std::cout << "Time taken: " << elapsed.count() << " seconds.\n";

        // Clear the buffer for the next communication
        shm1[0] = '\0';
        std::cout << "Buffer cleared for next communication." << std::endl;

        // Notify the dispatcher that the response has been read
        sem_post(sem_request);
        std::cout << "Dispatcher notified that the response has been read." << std::endl;
    }

    if (shmdt(shm1) == -1)
    {
        std::cerr << "Error: Failed to detach shared memory!" << std::endl;
    }
}
