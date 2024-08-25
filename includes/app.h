#ifndef __APP__HEADER__
#define __APP__HEADER__

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define R_END 0
#define W_END 1

#define SLAVE_OUTPUT_MAX_LEN 256

#define MAX_NUMBER_OF_SLAVES 5

#define SLAVE_BIN_NAME "./slave"
char* const slave_args[] = {SLAVE_BIN_NAME, NULL};

typedef struct {
    int in[2];
    int out[2];
} slave_pipes;

typedef struct {
    pid_t pid;
    char finished_job;
    slave_pipes pipes;
} slave_worker;

#endif
