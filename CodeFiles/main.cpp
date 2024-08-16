#include "../HeaderFiles/client.h"
#include "../HeaderFiles/dispatcher.h"
#include "../HeaderFiles/server.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>

int main()
{
    key_t key1 = ftok("shmfile1", 65);
    key_t key2 = ftok("shmfile2", 66);

    int shmid1 = shmget(key1, 1024, 0666 | IPC_CREAT);
    int shmid2 = shmget(key2, 1024, 0666 | IPC_CREAT);

    if (fork() == 0)
    {
        dispatcher(shmid1, shmid2);
    }
    else if (fork() == 0)
    {
        server(shmid2);
    }
    else
    {
        // Μπορούμε να δημιουργήσουμε περισσότερους πελάτες αν θέλουμε
        if (fork() == 0)
        {
            client(shmid1);
        }

        wait(nullptr); // Αναμονή για να ολοκληρωθούν οι διεργασίες
    }

    shmctl(shmid1, IPC_RMID, nullptr);
    shmctl(shmid2, IPC_RMID, nullptr);

    return 0;
}
