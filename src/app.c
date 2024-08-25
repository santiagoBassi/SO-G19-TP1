#include <app.h>
#include <utils.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

int main(int argc, char * argv[]){
    if (argc < 2) {
        fprintf(stderr, "Error: input files not provided.\n");
        return -1;
    }

    slave_worker slave_workers[MAX_NUMBER_OF_SLAVES]; 

    int num_file_args = argc - 1;
    const int num_slaves = min(MAX_NUMBER_OF_SLAVES, num_file_args);

    for (int i = 0; i < num_slaves; i++) {
        int pipe_status_in = pipe(slave_workers[i].pipes.in);
        int pipe_status_out = pipe(slave_workers[i].pipes.out);

        if (pipe_status_in == -1 || pipe_status_out == -1) {
           fprintf(stderr, "Error: could not create In/Out pipes to slaves.\n");
           return -1;
        }

        pid_t pid = fork();
        if (pid < -1) {
           fprintf(stderr, "Error: could not create slave process.\n");
           return -1;
        }

        if (pid == 0) {
            close(slave_workers[i].pipes.in[0]);    // in child close r-end of IN pipe
            close(slave_workers[i].pipes.out[1]);   // in child close w-end of OUT pipe 

            close(STDIN_FILENO);
            int dup_status_in = dup(slave_workers[i].pipes.in[1]);

            close(STDOUT_FILENO);
            int dup_status_out = dup(slave_workers[i].pipes.out[0]);

            if (dup_status_in == -1 || dup_status_out == -1) {
                fprintf(stderr, "Error: could not create In/Out pipes to slaves.\n");
                return -1;
            }

            int ret = execv(SLAVE_BIN_NAME, slave_args);
            if (ret < -1) {
                fprintf(stderr, "Error: could not create slave process.\n");
                return -1;
            }
        }

        close(slave_workers[i].pipes.in[1]);       // in parent close w-end of in pipe 
        close(slave_workers[i].pipes.out[0]);      // in parent close r-end of out pipe

        slave_workers[i].pid = pid;
    }

    int file_index = 0;
    while (file_index < num_file_args) {
        fd_set to_read_fds;
        fd_set to_write_fds;

        FD_ZERO(&read_fds, &write_fds);

        int max_fd = 0;
        for (int j = 0; j < num_slaves; j++) {
            max_fd = (slave_workers[j].pipes.in[0] > max_fd) ? slave_workers[j].pipes.in[0] : max_fd;
            max_fd = (slave_workers[j].pipes.out[1] > max_fd) ? slave_workers[j].pipes.out[1] : max_fd;

            FD_SET(slave_workers[j].pipes.in[0], &to_write_fds);
            FD_SET(slave_workers[j].pipes.out[1], &to_read_fds);
        }

        int select_status = select(max_fd, &to_read_fds, &to_write_fds, NULL, NULL); 

        for (int j = 0; j < num_slaves; j++) {
            if (FD_ISSET(slave_workers[j].pipes.in[0], &to_write_fds)) {
                write(slave_workers[j].pipes.in[0], argv[file_index + 1], strlen(argv[file_index + 1]));
            }

            if (FD_ISSET(slave_workers[j].pipes.out[1], &to_read_fds)) {
                file_index++;
            }
        }
    }

    for (int i = 0; i < num_slaves; i++) {
        close(slave_workers[i].pipes.in[0]);
        close(slave_workers[i].pipes.out[1]);
    }

    return 0;
}
