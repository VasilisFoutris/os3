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

        sem_wait(sem_request); // Wait for a request from the client
        std::cout << "Dispatcher detected request." << std::endl;

        if (shm1[0] != '\0')
        {
            std::cout << "Dispatcher received request: " << shm1 << std::endl;

            std::strcpy(shm2, shm1); // Copy request to server
            shm1[0] = '\0';          // Clear shm1 buffer
            std::cout << "Request copied to server shared memory." << std::endl;

            sem_post(sem_response); // Notify server
            std::cout << "Server notified of request." << std::endl;

            sem_wait(sem_request); // Wait for server response
            std::cout << "Server responded to request." << std::endl;

            std::cout << "Dispatcher sending response: " << shm2 << std::endl;
            std::strcpy(shm1, shm2); // Send response back to client
            shm2[0] = '\0';          // Clear shm2 buffer
            std::cout << "Response copied to client shared memory." << std::endl;

            sem_post(sem_response); // Notify client
            std::cout << "Client notified of response." << std::endl;
        }
        else
        {
            std::cout << "No request to process." << std::endl;
        }

        usleep(100000); // Avoid CPU overload
    }

    if (shmdt(shm1) == -1 || shmdt(shm2) == -1)
    {
        std::cerr << "Error: Failed to detach shared memory!" << std::endl;
    }
}
