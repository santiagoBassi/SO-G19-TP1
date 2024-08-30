#ifndef __SHARED_DATA__HEADER__
#define __SHARED_DATA__HEADER__

#include <sys/types.h>

#define FILENAME_MAX_LEN 256
#define HASH_LEN 33 // 32 chars + 1 null terminated
#define SHARED_NAME_MAX_LEN 256

typedef struct {
    char filename[FILENAME_MAX_LEN];
    char hash[HASH_LEN];
    pid_t slave_pid;
} shared_data;

#endif
