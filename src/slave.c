// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <slave.h>
#include <stdio.h>
#include <ctype.h>

int is_valid_input(const char* input) {
    for (int i = 0; input[i] != '\0'; i++)
        if (!isalnum(input[i]) && input[i] != '.' && input[i] != '/' && input[i] != '_')
            return 0;

    return 1;
}

int main(void)
{
    char line[FILE_NAME_MAX_LEN] = {0};
    char command[COMMAND_MAX_LEN];
    char bufferHash[MD5_OUTPUT_BUFFER_SIZE];

    while (fgets(line, FILE_NAME_MAX_LEN, stdin) != NULL)
    {
        int len = strlen(line);

        if (len <= 1) break;
        line[len - 1] = '\0';

        if (!is_valid_input(line)) {
            printf("Error: invalid input = %s\n", line);
            break;
        }

        snprintf(command, COMMAND_MAX_LEN, "md5sum %s", line);

        FILE* output = popen(command, "r");

        fgets(bufferHash, MD5_OUTPUT_BUFFER_SIZE, output);
        len = strlen(bufferHash);
        if (len <= 1) break;
        bufferHash[len - 1] = '\0';

        int status = pclose(output);

        if (status == 0)
            printf("%s\n", bufferHash);
        else 
            printf("Error: md5sum error for = %s\n", line);

        fflush(stdout);
    }

    return 0;
}
