#include <slave.h>
#include <stdio.h>
#include <unistd.h>

/*
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    while ((linelen = getline(&line, &linecap, stdin)) > 0) {
        line[linelen - 1] = '\0';

        printf("%s", line);

        fflush(stdout);
    }

    return 0;
*/
int main(void) {
    char bufferHash[BUFFER_SIZE]={0};
    
    char * argv[3] = {NULL};
    argv[0] = MD5_PATH;

    int pipeFd[2];

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    while ((linelen = getline(&line, &linecap, stdin)) > 0) {
        line[linelen - 1] = '\0';

        int p1 = pipe(pipeFd);
        if(p1 == -1){
            fprintf(stderr, "Error: Failed to create pipe");
            //ACA QUE HACEMOS?? 
        }
        
        int pid = fork();
        
        if (pid != 0) {
            close(pipeFd[W_END]);

            waitpid(pid, NULL, 0);

            int charsRead = read(pipeFd[R_END], bufferHash, BUFFER_SIZE);
            bufferHash[charsRead - 1] = 0;
            printf("%s", bufferHash);
            fflush(stdout);
             
            bufferHash[0] = 0;
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
