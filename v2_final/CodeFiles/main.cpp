#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../HeaderFiles/dispatcher.h"
#include "../HeaderFiles/server.h"
#include "../HeaderFiles/client.h"

// Function prototypes
void dispatcher(int shm_key, int sem_key);
void server(int shm_key, int sem_key);
void client(int shm_key, int sem_key);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <SHM_KEY> <SEM_KEY> <NUM_CLIENTS>" << std::endl;
        return 1;
    }

    key_t shm_key = atoi(argv[1]);
    key_t sem_key = atoi(argv[2]);
    int num_clients = atoi(argv[3]);

    std::cout << "Main process started with SHM_KEY: " << shm_key << ", SEM_KEY: " << sem_key << ", NUM_CLIENTS: " << num_clients << std::endl;

    pid_t pid;

    // Fork and execute the dispatcher process
    std::cout << "Forking dispatcher process..." << std::endl;
    pid = fork();
    if (pid == 0) {
        std::cout << "Dispatcher process starting." << std::endl;
        dispatcher(shm_key, sem_key);
        std::cout << "Dispatcher process exiting." << std::endl;
        return 0; // Exit child process
    } else if (pid < 0) {
        perror("fork");
        return 1;
    }
    std::cout << "Dispatcher process forked with PID: " << pid << std::endl;

    // Fork and execute the server process
    std::cout << "Forking server process..." << std::endl;
    pid = fork();
    if (pid == 0) {
        std::cout << "Server process starting." << std::endl;
        server(shm_key, sem_key);
        std::cout << "Server process exiting." << std::endl;
        return 0; // Exit child process
    } else if (pid < 0) {
        perror("fork");
        return 1;
    }
    std::cout << "Server process forked with PID: " << pid << std::endl;

    sleep(1); // Ensure dispatcher and server are started
    std::cout << "Main process sleeping for 1 second to ensure dispatcher and server are started." << std::endl;

    // Fork and execute client processes
    for (int i = 0; i < num_clients; ++i) {
        std::cout << "Forking client process " << i + 1 << "..." << std::endl;
        pid_t client_pid = fork();
        if (client_pid == 0) {
            std::cout << "Client process " << getpid() << " starting." << std::endl;
            client(shm_key, sem_key);
            std::cout << "Client process " << getpid() << " exiting." << std::endl;
            return 0; // Exit child process
        } else if (client_pid < 0) {
            perror("fork");
            return 1;
        }
        std::cout << "Client process forked with PID: " << client_pid << std::endl;
    }

    // Wait for all child processes to finish
    std::cout << "Waiting for all child processes to finish." << std::endl;
    for (int i = 0; i < num_clients + 2; ++i) {
        wait(nullptr);
    }
    std::cout << "All child processes have finished." << std::endl;

    return 0;
}
