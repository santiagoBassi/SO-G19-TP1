#include "app.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[]){
    if (argc < 2) {
        fprintf(stderr, "Error: se deben proporcionar archivos de entrada al programa\n");
        return -1;
    }

    pid_t child_pids[MAX_NUMBER_OF_SLAVES]; 
    int pipefds_to_child[MAX_NUMBER_OF_SLAVES];

    int num_file_args = argc - 1;
    const int num_slaves = min(MAX_NUMBER_OF_SLAVES, num_file_args);

    for (int i = 0; i < num_slaves; i++) {
        pid_t pid = fork();
        if (pid < -1) {
           fprintf(stderr, "Error: no se pudo crear uno de los procesos slave");
           return -1;
        }

        if (pid == 0) {
            int ret = execv(slave_bin_name, slave_args);
            if (ret < -1) {
                fprintf(stderr, "Error: no se pudo crear uno de los procesos slave");
                return -1;
            }
        }

        child_pids[i] = pid;
    }

    

    return 0;
}
