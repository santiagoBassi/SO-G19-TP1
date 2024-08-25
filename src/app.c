#include <app.h>
#include <utils.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

int create_slaves(slave_worker* slave_workers, int num_slaves) {
    for (int i = 0; i < num_slaves; i++) {
        int pipe_status_in = pipe(slave_workers[i].pipes.in);
        int pipe_status_out = pipe(slave_workers[i].pipes.out);

        if (pipe_status_in == -1 || pipe_status_out == -1) {
           fprintf(stderr, "Error: could not create In/Out pipes to slaves.\n");
           return -1;
        }

        pid_t pid = fork();
        if (pid == -1) return -1;

        if (pid == 0) {
            close(slave_workers[i].pipes.in[W_END]);
            close(slave_workers[i].pipes.out[R_END]);

            close(STDIN_FILENO);
            int dup_status_in = dup(slave_workers[i].pipes.in[R_END]);
            close(slave_workers[i].pipes.in[R_END]);

            close(STDOUT_FILENO);
            int dup_status_out = dup(slave_workers[i].pipes.out[W_END]);
            close(slave_workers[i].pipes.out[W_END]);

            if (dup_status_in == -1 || dup_status_out == -1) return -1;

            int ret = execv(SLAVE_BIN_NAME, slave_args);
            if (ret == -1) return -1;
        }

        close(slave_workers[i].pipes.in[R_END]);
        close(slave_workers[i].pipes.out[W_END]);

        slave_workers[i].pid = pid;
        slave_workers[i].finished_job = 1;
    }

    return 0;
}

int execute_jobs_on_files(slave_worker* slave_workers, int num_slaves, char* files[], int num_file_args) {
    int write_file_index = 0;
    int read_file_index = 0;

    while (read_file_index < num_file_args) {
        fd_set to_read_fds;
        fd_set to_write_fds;
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 200;

        FD_ZERO(&to_read_fds);
        FD_ZERO(&to_write_fds);

        int max_fd = 0;
        for (int i = 0; i < num_slaves; i++) {
            max_fd = (slave_workers[i].pipes.in[W_END] > max_fd) ? slave_workers[i].pipes.in[W_END] : max_fd;
            max_fd = (slave_workers[i].pipes.out[R_END] > max_fd) ? slave_workers[i].pipes.out[R_END] : max_fd;

            FD_SET(slave_workers[i].pipes.in[W_END], &to_write_fds);
            FD_SET(slave_workers[i].pipes.out[R_END], &to_read_fds);
        }

        select(max_fd + 1, &to_read_fds, &to_write_fds, NULL, &timeout); 

        for (int i = 0; i < num_slaves; i++) {
            if (write_file_index < num_file_args && slave_workers[i].finished_job && FD_ISSET(slave_workers[i].pipes.in[W_END], &to_write_fds)) {
                write(slave_workers[i].pipes.in[W_END], files[write_file_index], strlen(files[write_file_index]));
                write(slave_workers[i].pipes.in[W_END], "\n", 1);

                slave_workers[i].finished_job = 0;

                write_file_index++;
            }

            if (FD_ISSET(slave_workers[i].pipes.out[R_END], &to_read_fds)) {
                char buff[SLAVE_OUTPUT_MAX_LEN];
                int chars_read = read(slave_workers[i].pipes.out[R_END], buff, SLAVE_OUTPUT_MAX_LEN);
                buff[chars_read] = '\0';

                printf("%d: %s\n", slave_workers[i].pid, buff);

                slave_workers[i].finished_job = 1;

                read_file_index++;
            }
        }
    }

    return 0;
}

int close_pipes(slave_worker* slave_workers, int num_slaves) {
    for (int i = 0; i < num_slaves; i++) {
        close(slave_workers[i].pipes.in[W_END]);
        close(slave_workers[i].pipes.out[R_END]);
    }

    return 0;
}

int main(int argc, char * argv[]){
    if (argc < 2) {
        fprintf(stderr, "Error: input files not provided.\n");
        return -1;
    }

    slave_worker slave_workers[MAX_NUMBER_OF_SLAVES]; 

    int num_file_args = argc - 1;
    const int num_slaves = min(MAX_NUMBER_OF_SLAVES, num_file_args);

    if (create_slaves(slave_workers, num_slaves) == -1) {
        fprintf(stderr, "Error: could not create slaves");
        return -1;
    }

    execute_jobs_on_files(slave_workers, num_slaves, argv + 1, num_file_args);

    close_pipes(slave_workers, num_slaves);

    return 0;
}
