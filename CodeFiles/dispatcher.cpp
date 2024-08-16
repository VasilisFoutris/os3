#include "../HeaderFiles/dispatcher.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <unistd.h>
#include <semaphore.h>

void dispatcher(int shmid1, int shmid2, sem_t *sem_request, sem_t *sem_response)
{
    // Attach shared memory
    char *shm1 = (char *)shmat(shmid1, nullptr, 0);
    char *shm2 = (char *)shmat(shmid2, nullptr, 0);

    if (shm1 == (char *)-1 || shm2 == (char *)-1)
    {
        std::cerr << "Error: Failed to attach shared memory!" << std::endl;
        return;
    }

    while (true)
    {
        // Wait for a request from the client
        sem_wait(sem_request);

        if (shm1[0] != '\0')
        {
            std::cout << "Dispatcher received request: " << shm1 << std::endl;

            // Copy the request to the shared memory for the server
            std::strcpy(shm2, shm1);
            shm1[0] = '\0'; // Clear buffer in shm1

            // Signal the server that a request is available
            sem_post(sem_response);

            // Wait for the server to respond
            sem_wait(sem_request);

            std::cout << "Dispatcher sending response: " << shm2 << std::endl;

            // Send the response back to the client
            std::strcpy(shm1, shm2);
            shm2[0] = '\0'; // Clear buffer in shm2

            // Signal the server that the response has been read
            sem_post(sem_response);
        }

        usleep(100000); // Wait 100ms to avoid CPU overload
    }

    // Clean up resources on exit
    if (shmdt(shm1) == -1 || shmdt(shm2) == -1)
        std::cerr << "Error: Failed to detach shared memory!" << std::endl;
}
