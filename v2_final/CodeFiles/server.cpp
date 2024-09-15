#include <iostream>
#include <fstream>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstring>
#include "../HeaderFiles/server.h"

void handle_request(RequestServer *request, const std::string &filename)
{
    std::cout << "Handling request for line number: " << request->line_number << std::endl;

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    int current_line = 0;
    while (std::getline(file, line))
    {
        if (++current_line == request->line_number)
        {
            strncpy(request->line_content, line.c_str(), sizeof(request->line_content) - 1);
            request->line_content[sizeof(request->line_content) - 1] = '\0'; // Ensure null termination
            std::cout << "Retrieved line " << request->line_number << ": " << request->line_content << std::endl;
            break;
        }
    }

    if (current_line != request->line_number) 
    {
        std::cerr << "Requested line number " << request->line_number << " not found in file." << std::endl;
    }
}

void server(int shm_key, int sem_key)
{
    std::cout << "Server starting with SHM_KEY: " << shm_key << " and SEM_KEY: " << sem_key << std::endl;

    int shm_id = shmget(shm_key, sizeof(RequestServer), 0666);
    int sem_id = semget(sem_key, 4, 0666);

    if (shm_id == -1 || sem_id == -1)
    {
        perror("shmget or semget");
        exit(1);
    }

    std::cout << "Shared memory ID: " << shm_id << std::endl;
    std::cout << "Semaphore ID: " << sem_id << std::endl;

    RequestServer *request = (RequestServer *)shmat(shm_id, nullptr, 0);
    if (request == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }

    std::cout << "Attached to shared memory." << std::endl;

    while (true)
    {
        struct sembuf sop;

        std::cout << "Waiting for server semaphore (P operation)." << std::endl;
        // Wait for server semaphore (P operation)
        sop.sem_num = 1; 
        sop.sem_op = -1; 
        sop.sem_flg = 0;
        semop(sem_id, &sop, 1);

        std::cout << "Server semaphore acquired." << std::endl;

        handle_request(request, "random_chars.txt");

        std::cout << "Signaling dispatcher semaphore (V operation)." << std::endl;
        // Signal dispatcher semaphore (V operation)
        sop.sem_num = 2; 
        sop.sem_op = 1;  
        semop(sem_id, &sop, 1);

        std::cout << "Dispatcher semaphore signaled." << std::endl;
    }

    shmdt(request);
    std::cout << "Detached from shared memory and exiting server." << std::endl;
}
