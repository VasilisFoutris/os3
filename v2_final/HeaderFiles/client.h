#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>

struct RequestClient {
    int line_number;
    pid_t client_id;
    char line_content[256];
};

#endif // CLIENT_H
