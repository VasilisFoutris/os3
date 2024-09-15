#include "../HeaderFiles/dispatcher.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <unistd.h>
#include <semaphore.h>

void dispatcher(int shmid1, int shmid2, sem_t *sem_request, sem_t *sem_response, sem_t *sem_helper)
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

        sem_wait(sem_helper);
        if (shm1[0] != '\0')
        {
            std::cout << "Dispatcher received request: " << shm1 << std::endl;

            std::strcpy(shm2, shm1);
            shm1[0] = '\0';
            std::cout << "Request copied to server shared memory." << std::endl;

            sem_post(sem_request);
            std::cout << "Server notified of request." << std::endl;

            sem_wait(sem_response);

            std::cout << "Dispatcher received response from server: " << shm2 << std::endl;

            sem_post(sem_helper);
        }
        else
        {
            std::cout << "No request to process." << std::endl;
        }

        usleep(100000);
    }

    if (shmdt(shm1) == -1 || shmdt(shm2) == -1)
    {
        std::cerr << "Error: Failed to detach shared memory!" << std::endl;
    }
}
