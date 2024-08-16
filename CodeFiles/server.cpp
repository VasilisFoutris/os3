#include "../HeaderFiles/server.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <fstream>
#include <unistd.h>

void server(int shmid2)
{
    char *shm2 = (char *)shmat(shmid2, nullptr, 0);

    while (true)
    {
        if (shm2[0] != '\0')
        { // Έλεγχος αν υπάρχει αίτημα από τον Dispatcher
            int lineNumber = atoi(shm2);

            // Άνοιγμα του αρχείου και ανάγνωση της ζητούμενης γραμμής
            std::ifstream file("textfile.txt");
            std::string line;
            for (int i = 0; i < lineNumber && std::getline(file, line); ++i)
                ;

            // Αποστολή της γραμμής πίσω στον Dispatcher
            std::strcpy(shm2, line.c_str());

            // Καθαρισμός του buffer για την επόμενη επικοινωνία
            file.close();
            shm2[0] = '\0';
        }
        sleep(1); // Καθυστέρηση για την αποφυγή υπερφόρτωσης της CPU
    }
}
