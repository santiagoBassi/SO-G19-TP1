#include <slave.h>
#include <stdio.h>
#include <unistd.h>

int main(void){
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    while ((linelen = getline(&line, &linecap, stdin)) > 0) {
        line[linelen - 1] = '\0';

        printf("%s", line);

        fflush(stdout);
    }

    return 0;
    /*
    char buffer[256];

    while(TRUE){
        read(STDIN_FILENO, buffer, 256);
        printf("%s\n", buffer);
        int pid = fork();
        if(pid != 0){
            printf("Soy el padre");//tengo que acomodar los fd y llamar al
        } else{
            printf("Soy el hijo");//execev() ... 
        }
        return 0;
    }

    return 0;
    */
}
