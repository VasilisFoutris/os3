#ifndef SERVER_H
#define SERVER_H

#include <semaphore.h>

void server(int shmid2, sem_t *sem_request, sem_t *sem_response);

#endif
