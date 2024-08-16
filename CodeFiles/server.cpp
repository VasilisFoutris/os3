#include "../HeaderFiles/server.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <cstdlib> // For std::strtol
#include <cerrno>  // For errno

void server(int shmid2, sem_t *sem_request, sem_t *sem_response)
{
    char *shm2 = (char *)shmat(shmid2, nullptr, 0);

    if (shm2 == (char *)-1)
    {
        std::cerr << "Error: Failed to attach shared memory!" << std::endl;
        return;
    }

    while (true)
    {
        sem_wait(sem_request); // Wait for a request from dispatcher

        if (shm2[0] != '\0') // Check if there's a request
        {
            char *endptr;
            errno = 0; // Reset errno before conversion
            int lineNumber = std::strtol(shm2, &endptr, 10);

            if (*endptr != '\0' || lineNumber <= 0 || errno != 0)
            {
                std::cerr << "Error: Invalid line number! LineNumber: " << lineNumber << std::endl;
                std::strcpy(shm2, "Error: Invalid line number!");
                shm2[0] = '\0';         // Clear buffer
                sem_post(sem_response); // Notify dispatcher
                continue;
            }

            shm2[0] = '\0'; // Clear buffer before processing

            std::ifstream file("TextInputGenerator/random_chars.txt");

            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file!" << std::endl;
                std::strcpy(shm2, "Error: Could not open file!");
                sem_post(sem_response);
                continue;
            }

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
            }
            else
            {
                std::cerr << "Error: Line " << lineNumber << " not found!" << std::endl;
                std::strcpy(shm2, "Error: Line not found!");
            }

            file.close();
            shm2[0] = '\0';         // Clear buffer
            sem_post(sem_response); // Notify dispatcher
        }
    }

    shmdt(shm2);
}
