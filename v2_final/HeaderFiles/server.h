#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>

struct RequestServer {
    int line_number;
    pid_t client_id;
    char line_content[256];
};

#endif // SERVER_H
