#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "client.h"
#include "dispatcher.h"
#include "server.h"
    
#define SM1_KEY 1234
#define SM2_KEY 2345
#define SEM1_KEY 5678
#define SEM2_KEY 6789

void create_dispatcher();
void create_server();
void create_client();

#endif
