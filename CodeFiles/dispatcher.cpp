#include "../HeaderFiles/dispatcher.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <unistd.h>

void dispatcher(int shmid1, int shmid2)
{
    char *shm1 = (char *)shmat(shmid1, nullptr, 0);
    char *shm2 = (char *)shmat(shmid2, nullptr, 0);

    while (true)
    {
        if (shm1[0] != '\0')
        {                            // Έλεγχος αν υπάρχει αίτημα από κάποιον πελάτη
            std::strcpy(shm2, shm1); // Προώθηση του αιτήματος στον Server
            shm1[0] = '\0';

            while (shm2[0] != '\0')
                sleep(1); // Αναμονή για την απάντηση του Server

            // Αποστολή της απάντησης πίσω στον πελάτη
            std::strcpy(shm1, shm2);
            shm2[0] = '\0';
        }
        sleep(1); // Καθυστέρηση για την αποφυγή υπερφόρτωσης της CPU
    }
}
