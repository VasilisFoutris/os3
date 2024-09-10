#include "../HeaderFiles/client.h"
#include "../HeaderFiles/dispatcher.h"
#include "../HeaderFiles/server.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <cstring>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
    key_t key1 = ftok("shmfile1", 65);
    key_t key2 = ftok("shmfile2", 66);

    if (key1 == -1 || key2 == -1)
    {
        std::cerr << "Error: Failed to generate keys! " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "Generated keys: " << key1 << ", " << key2 << std::endl;

    int shmid1 = shmget(key1, 1024, 0666 | IPC_CREAT);
    int shmid2 = shmget(key2, 1024, 0666 | IPC_CREAT);

    if (shmid1 == -1 || shmid2 == -1)
    {
        std::cerr << "Error: Failed to get shared memory segments! " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "Shared memory segments created with IDs: " << shmid1 << ", " << shmid2 << std::endl;

    // Initialize semaphores
    sem_t *sem_request = sem_open("/sem_request", O_CREAT, 0644, 0);
    sem_t *sem_response = sem_open("/sem_response", O_CREAT, 0644, 0);
    sem_t *sem_child = sem_open("/sem_child", O_CREAT, 0644, 1);   // Initialize to 1 to allow the first client to enter
    sem_t *sem_helper = sem_open("/sem_helper", O_CREAT, 0644, 0); // Dispatcher waits initially

    if (sem_request == SEM_FAILED || sem_response == SEM_FAILED || sem_child == SEM_FAILED || sem_helper == SEM_FAILED)
    {
        std::cerr << "Error: Failed to create semaphores! " << strerror(errno) << std::endl;
        return 1;
    }

    std::cout << "Semaphores created: /sem_request, /sem_response" << std::endl;

    pid_t pid = fork();
    if (pid == -1)
    {
        std::cerr << "Error: Failed to fork dispatcher process! " << strerror(errno) << std::endl;
        return 1;
    }

    if (pid == 0)
    {
        // Child process for dispatcher
        std::cout << "Dispatcher process (PID: " << getpid() << ") starting." << std::endl;
        dispatcher(shmid1, shmid2, sem_request, sem_response, sem_helper);
        sem_close(sem_request);
        sem_close(sem_response);
        std::cout << "Dispatcher process (PID: " << getpid() << ") exiting." << std::endl;
        return 0;
    }

    pid = fork();
    if (pid == -1)
    {
        std::cerr << "Error: Failed to fork server process! " << strerror(errno) << std::endl;
        return 1;
    }

    if (pid == 0)
    {
        // Child process for server
        std::cout << "Server process (PID: " << getpid() << ") starting." << std::endl;
        server(shmid2, sem_request, sem_response);
        sem_close(sem_request);
        sem_close(sem_response);
        std::cout << "Server process (PID: " << getpid() << ") exiting." << std::endl;
        return 0;
    }

    // Fork 10 client processes
    const int num_clients = 10;
    for (int i = 0; i < num_clients; ++i)
    {
        pid = fork();
        if (pid == -1)
        {
            std::cerr << "Error: Failed to fork client process! " << strerror(errno) << std::endl;
            return 1;
        }

        if (pid == 0)
        {
            // Child process for client
            std::cout << "Client process (PID: " << getpid() << ") starting." << std::endl;
            client(shmid1, sem_request, sem_response, sem_child, sem_helper);
            sem_close(sem_request);
            sem_close(sem_response);
            std::cout << "Client process (PID: " << getpid() << ") exiting." << std::endl;
            return 0;
        }
    }

    // Parent process
    std::cout << "Parent process (PID: " << getpid() << ") waiting for child processes to exit." << std::endl;
    int status;
    for (int i = 0; i < num_clients + 2; ++i) // Wait for dispatcher, server, and all clients
    {
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            std::cerr << "Child process exited with an error." << std::endl;
        }
    }

    // Cleanup shared memory
    if (shmctl(shmid1, IPC_RMID, nullptr) == -1 || shmctl(shmid2, IPC_RMID, nullptr) == -1)
    {
        std::cerr << "Error: Failed to remove shared memory segments! " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "Shared memory segments removed." << std::endl;

    // Cleanup semaphores
    if (sem_close(sem_request) == -1 || sem_close(sem_response) == -1 || sem_close(sem_child) == -1 || sem_close(sem_helper) == -1)
    {
        std::cerr << "Error: Failed to close semaphores! " << strerror(errno) << std::endl;
        return 1;
    }
    if (sem_unlink("/sem_request") == -1 || sem_unlink("/sem_response") == -1 || sem_unlink("/sem_child") == -1 || sem_unlink("/sem_helper") == -1)
    {
        std::cerr << "Error: Failed to unlink semaphores! " << strerror(errno) << std::endl;
        return 1;
    }

    std::cout << "Semaphores closed and unlinked." << std::endl;

    return 0;
}
