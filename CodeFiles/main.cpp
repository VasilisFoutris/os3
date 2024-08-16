#include "../HeaderFiles/client.h"
#include "../HeaderFiles/dispatcher.h"
#include "../HeaderFiles/server.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <cstring> // For strerror
#include <errno.h> // For errno
#include <semaphore.h>
#include <fcntl.h>    // For O_CREAT
#include <sys/stat.h> // For mode constants

int main()
{
    key_t key1 = ftok("shmfile1", 65);
    key_t key2 = ftok("shmfile2", 66);

    if (key1 == -1 || key2 == -1)
    {
        std::cerr << "Error: Failed to generate keys! " << strerror(errno) << std::endl;
        return 1;
    }

    int shmid1 = shmget(key1, 1024, 0666 | IPC_CREAT);
    int shmid2 = shmget(key2, 1024, 0666 | IPC_CREAT);

    if (shmid1 == -1 || shmid2 == -1)
    {
        std::cerr << "Error: Failed to get shared memory segments! " << strerror(errno) << std::endl;
        return 1;
    }

    // Initialize semaphores
    sem_t *sem_request = sem_open("/sem_request", O_CREAT, 0644, 0);
    sem_t *sem_response = sem_open("/sem_response", O_CREAT, 0644, 0);

    if (sem_request == SEM_FAILED || sem_response == SEM_FAILED)
    {
        std::cerr << "Error: Failed to create semaphores!" << std::endl;
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        std::cerr << "Error: Failed to fork! " << strerror(errno) << std::endl;
        return 1;
    }

    if (pid == 0)
    {
        // Child process for dispatcher
        dispatcher(shmid1, shmid2, sem_request, sem_response);
        sem_close(sem_request);
        sem_close(sem_response);
        return 0;
    }

    pid = fork();
    if (pid == -1)
    {
        std::cerr << "Error: Failed to fork! " << strerror(errno) << std::endl;
        return 1;
    }

    if (pid == 0)
    {
        // Child process for server
        server(shmid2, sem_request, sem_response);
        sem_close(sem_request);
        sem_close(sem_response);
        return 0;
    }

    pid = fork();
    if (pid == -1)
    {
        std::cerr << "Error: Failed to fork! " << strerror(errno) << std::endl;
        return 1;
    }

    if (pid == 0)
    {
        // Child process for client
        client(shmid1, sem_request, sem_response);
        sem_close(sem_request);
        sem_close(sem_response);
        return 0;
    }

    // Parent process
    while (wait(nullptr) > 0)
        ;

    // Cleanup shared memory
    shmctl(shmid1, IPC_RMID, nullptr);
    shmctl(shmid2, IPC_RMID, nullptr);

    // Cleanup semaphores
    sem_close(sem_request);
    sem_close(sem_response);
    sem_unlink("/sem_request");
    sem_unlink("/sem_response");

    return 0;
}
