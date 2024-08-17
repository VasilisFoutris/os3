#include "../HeaderFiles/dispatcher.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <unistd.h>
#include <semaphore.h>

void dispatcher(int shmid1, int shmid2, sem_t *sem_request, sem_t *sem_response)
{
    char *shm1 = (char *)shmat(shmid1, nullptr, 0);
    char *shm2 = (char *)shmat(shmid2, nullptr, 0);

    if (shm1 == (char *)-1 || shm2 == (char *)-1)
    {
        std::cerr << "Error: Failed to attach shared memory!" << std::endl;
        return;
    }

    while (true)
    {
        std::cout << "Dispatcher is waiting for a request..." << std::endl;

        // Wait for a request from the client
        sem_wait(sem_request);
        std::cout << "Dispatcher detected request." << std::endl;

        if (shm1[0] != '\0')
        {
            std::cout << "Dispatcher received request: " << shm1 << std::endl;

            // Copy the request to shared memory for the server
            std::strcpy(shm2, shm1);
            shm1[0] = '\0'; // Clear buffer in shm1
            std::cout << "Request copied to server shared memory." << std::endl;

            // Notify the server that a request is available
            sem_post(sem_response);
            std::cout << "Server notified of request." << std::endl;

            // Wait for the server to respond
            sem_wait(sem_request);
            std::cout << "Server responded to request." << std::endl;

            std::cout << "Dispatcher sending response: " << shm2 << std::endl;

            // Send the response back to the client
            std::strcpy(shm1, shm2);
            shm2[0] = '\0'; // Clear buffer in shm2
            std::cout << "Response copied to client shared memory." << std::endl;

            // Notify the client that the response is ready
            sem_post(sem_response);
            std::cout << "Client notified of response." << std::endl;
        }
        else
        {
            std::cout << "No request to process." << std::endl;
        }

        usleep(100000); // Wait 100ms to avoid CPU overload
    }

    if (shmdt(shm1) == -1 || shmdt(shm2) == -1)
    {
        std::cerr << "Error: Failed to detach shared memory!" << std::endl;
    }
}
