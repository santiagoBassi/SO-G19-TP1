#ifndef __APP__HEADER__
#define __APP__HEADER__

#include <stdio.h>
#include <unistd.h>

#define MAX_NUMBER_OF_SLAVES 5

#define SLAVE_BIN_NAME "./slave"
char* const slave_args[] = {SLAVE_BIN_NAME, NULL};

typedef struct {
    int in[2];
    int out[2];
} slave_pipes;

typedef struct {
    pid_t pid;
    slave_pipes pipes;
} slave_worker;

#endif
