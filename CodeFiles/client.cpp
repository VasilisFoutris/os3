#include "../HeaderFiles/client.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <chrono>
#include <unistd.h>

void client(int shmid1)
{
    char *shm1 = (char *)shmat(shmid1, nullptr, 0);

    while (true)
    {
        int lineNumber;
        std::cout << "Enter the line number: ";
        std::cin >> lineNumber;

        auto start = std::chrono::high_resolution_clock::now();

        std::sprintf(shm1, "%d", lineNumber);

        while (shm1[0] != '\0')
            sleep(1); // Αναμονή για την απάντηση από τον Dispatcher

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Line: " << shm1 << "\n";
        std::cout << "Time taken: " << elapsed.count() << " seconds.\n";

        shm1[0] = '\0'; // Καθαρισμός του buffer για την επόμενη επικοινωνία
    }
}
