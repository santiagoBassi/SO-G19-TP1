#include <slave.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    char * argv[3] = {NULL};
    argv[0] = MD5_PATH;

    int pipeFd[2];

    char line[FILE_NAME_MAX_LEN];

    while (fgets(line, FILE_NAME_MAX_LEN, stdin) != NULL) {
        line[strlen(line) - 1] = '\0';

        int pipe_status = pipe(pipeFd);
        if (pipe_status == -1){
            fprintf(stderr, "Error: Failed to create pipe");
            return -1;
        }
        
        int pid = fork();
        
        if (pid != 0) {
            close(pipeFd[W_END]);

            int md5_status;
            waitpid(pid, &md5_status, 0);

            if (md5_status != 0) {
                printf("Error: md5 produced an error for this file\n");
            } else {
                char bufferHash[MD5_OUTPUT_BUFFER_SIZE];
                int charsRead = read(pipeFd[R_END], bufferHash, MD5_OUTPUT_BUFFER_SIZE);
                bufferHash[charsRead - 1] = 0;
                printf("%s\n", bufferHash);
            }
            fflush(stdout);
             
            close(pipeFd[R_END]);
       } else {
            close(pipeFd[R_END]);
            close(STDOUT_FILENO);

            dup(pipeFd[W_END]);
            close(pipeFd[W_END]);

            argv[1] = line;
            execve(MD5_PATH, argv, NULL);
        }
    }

    return 0;
}
