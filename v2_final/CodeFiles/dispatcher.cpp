#include <sys/shm.h>
#include <sys/sem.h>
#include <iostream>
#include "../HeaderFiles/dispatcher.h"

void dispatcher(int shm_key, int sem_key)
{
    std::cout << "Dispatcher starting with SHM_KEY: " << shm_key << " and SEM_KEY: " << sem_key << std::endl;

    int shm_id = shmget(shm_key, sizeof(RequestDispatcher), 0666 | IPC_CREAT);
    int sem_id = semget(sem_key, 4, 0666 | IPC_CREAT);

    if (shm_id == -1 || sem_id == -1)
    {
        perror("shmget or semget");
        exit(1);
    }

    std::cout << "Shared memory ID: " << shm_id << std::endl;
    std::cout << "Semaphore ID: " << sem_id << std::endl;

    RequestDispatcher *request = (RequestDispatcher *)shmat(shm_id, nullptr, 0);
    if (request == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }

    std::cout << "Attached to shared memory." << std::endl;

    struct sembuf sop;

    // Initialize semaphores
    std::cout << "Initializing semaphores." << std::endl;
    semctl(sem_id, 0, SETVAL, 1); // Dispatcher semaphore
    semctl(sem_id, 1, SETVAL, 1); // Server semaphore
    semctl(sem_id, 2, SETVAL, 0); // Dispatcher semaphore
    semctl(sem_id, 3, SETVAL, 0); // Server semaphore

    while (true)
    {
        std::cout << "Waiting for dispatcher semaphore (P operation)." << std::endl;
        // Wait for dispatcher semaphore (P operation)
        sop.sem_num = 0; 
        sop.sem_op = -1; 
        sop.sem_flg = 0;
        semop(sem_id, &sop, 1);

        std::cout << "Dispatcher semaphore acquired." << std::endl;

        std::cout << "Signaling server semaphore (V operation)." << std::endl;
        // Signal server semaphore (V operation)
        sop.sem_num = 1; 
        sop.sem_op = 1;  
        semop(sem_id, &sop, 1);

        std::cout << "Server semaphore signaled." << std::endl;

        std::cout << "Signaling dispatcher semaphore (V operation)." << std::endl;
        // Signal dispatcher semaphore (V operation)
        sop.sem_num = 2; 
        sop.sem_op = 1;  
        semop(sem_id, &sop, 1);

        std::cout << "Dispatcher semaphore signaled." << std::endl;
    }

    shmdt(request);
    std::cout << "Detached from shared memory and exiting dispatcher." << std::endl;
}
