#ifndef __SLAVE__HEADER__
#define __SLAVE__HEADER__

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define FILE_NAME_MAX_LEN 256
#define COMMAND_MAX_LEN (FILE_NAME_MAX_LEN + 7)
#define HASH_LEN 32

#define MD5_OUTPUT_BUFFER_SIZE (FILE_NAME_MAX_LEN + HASH_LEN)

#define R_END 0
#define W_END 1

#endif
