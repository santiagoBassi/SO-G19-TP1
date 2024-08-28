#ifndef __APP__HEADER__
#define __APP__HEADER__

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define R_END 0
#define W_END 1

#define SLAVE_OUTPUT_MAX_LEN 256
#define FILENAME_MAX_LEN 256
#define HASH_LEN 33 // 32 chars + 1 null terminated

#define MAX_NUMBER_OF_SLAVES 5 
#define NUM_FILES_IN_INITIAL_JOB 2
#define SLAVE_OUTPUT_DELIM "\n"

#define OUTPUT_FILE_NAME "./result.txt"

#define SLAVE_BIN_NAME "./slave"
char* const slave_args[] = {SLAVE_BIN_NAME, NULL};

typedef struct {
    int in[2];
    int out[2];
} slave_pipes;

typedef struct {
    pid_t pid;

    char is_processing_job;
    int num_files_in_job;
    int num_files_processed_in_job;

    slave_pipes pipes;
} slave_worker;

typedef struct {
    char filename[FILENAME_MAX_LEN];
    char hash[HASH_LEN + 1];
    pid_t slave_pid;
} shared_data;

#endif
