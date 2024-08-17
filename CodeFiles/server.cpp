#include "../HeaderFiles/server.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>
#include <string>

void server(int shmid2, sem_t *sem_request, sem_t *sem_response)
{
    char *shm2 = (char *)shmat(shmid2, nullptr, 0);

    if (shm2 == (char *)-1)
    {
        std::cerr << "Error: Failed to attach shared memory!" << std::endl;
        return;
    }

    std::ifstream file("TextInputGenerator/random_chars.txt");
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file 'TextInputGenerator/random_chars.txt'!" << std::endl;
        std::strcpy(shm2, "Error: File not found or cannot be opened!");
        shm2[0] = '\0';         // Clear buffer
        sem_post(sem_response); // Notify dispatcher
        shmdt(shm2);            // Detach shared memory before returning
        return;
    }

    while (true)
    {
        std::cout << "Server is waiting for a request..." << std::endl;
        sem_wait(sem_request); // Wait for request from dispatcher
        std::cout << "Server detected request." << std::endl;

        if (shm2[0] != '\0') // Check if there's a request
        {
            char *endptr;
            errno = 0;
            int lineNumber = std::strtol(shm2, &endptr, 10);

            if (*endptr != '\0' || lineNumber <= 0 || errno != 0)
            {
                std::cerr << "Error: Invalid line number received: '" << shm2 << "'!" << std::endl;
                std::strcpy(shm2, "Error: Invalid line number!");
                shm2[0] = '\0';         // Clear buffer
                sem_post(sem_response); // Notify dispatcher
                continue;
            }

            shm2[0] = '\0'; // Clear buffer before processing
            file.clear();
            file.seekg(0, std::ios::beg); // Rewind file to beginning

            std::string line;
            bool lineFound = false;
            for (int i = 1; i <= lineNumber; ++i)
            {
                if (!std::getline(file, line))
                {
                    lineFound = false;
                    break;
                }
                lineFound = true;
            }

            if (lineFound)
            {
                std::strcpy(shm2, line.c_str());
                std::cout << "Line " << lineNumber << " found and copied to shared memory." << std::endl;
            }
            else
            {
                std::cerr << "Error: Line " << lineNumber << " not found in file!" << std::endl;
                std::strcpy(shm2, "Error: Line not found!");
            }

            file.close();
            sem_post(sem_response); // Notify dispatcher
        }
        else
        {
            std::cerr << "No request received." << std::endl;
        }
    }

    if (shmdt(shm2) == -1)
    {
        std::cerr << "Error: Failed to detach shared memory!" << std::endl;
    }
}
