#ifndef CLIENT_H
#define CLIENT_H

#include <semaphore.h>

void client(int shmid1, sem_t *sem_request, sem_t *sem_response, sem_t *sem_child, sem_t *sem_helper);

#endif
