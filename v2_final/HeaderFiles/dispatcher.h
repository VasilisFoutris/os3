#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <sys/types.h>

struct RequestDispatcher {
    int line_number;
    pid_t client_id;
    char line_content[256];
};

#endif // DISPATCHER_H
