#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <semaphore.h>

void dispatcher(int shmid1, int shmid2, sem_t *sem_request, sem_t *sem_response, sem_t *sem_helper);

#endif
