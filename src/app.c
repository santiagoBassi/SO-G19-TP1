// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <app.h>

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
            for(int j=0; j<i; j++){
                close(slave_workers[j].pipes.in[W_END]);
                close(slave_workers[j].pipes.out[R_END]);
            }
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
        slave_workers[i].is_processing_job = 0;
    }

    return 0;
}

int add_job_to_slave(char* files[], int num_files, slave_worker* slave) {
    for (int i = 0; i < num_files; i++) {
        write(slave->pipes.in[W_END], files[i], strlen(files[i]));
        write(slave->pipes.in[W_END], "\n", 1);
    }

    slave->num_files_in_job = num_files;
    slave->num_files_processed_in_job = 0;

    slave->is_processing_job = 1;

    return num_files;
}

int distribute_initial_jobs_to_slaves(slave_worker* slave_workers, int num_slaves, char* files[], int num_files) {
    int index_in_files = 0;

    for (int i = 0; i < num_slaves && index_in_files < num_files; i++)
        index_in_files += add_job_to_slave(files + index_in_files, min(NUM_FILES_IN_INITIAL_JOB, num_files - index_in_files), &slave_workers[i]); 

    return index_in_files;
}

void add_info_to_shared_data(shared_data* data, char* slave_output_on_file, pid_t slave_pid, sem_t* share_sem) {
    strncpy(data->filename, slave_output_on_file + HASH_LEN + 1, FILENAME_MAX_LEN);
    data->filename[FILENAME_MAX_LEN-1] = '\0';
    strncpy(data->hash, slave_output_on_file, HASH_LEN);
    data->hash[HASH_LEN - 1] = '\0';
    data->slave_pid = slave_pid;

    sem_post(share_sem);
}

int process_slave_output(char* slave_output, FILE* output_file, pid_t slave_pid, shared_data* shared_buffer, int files_added, sem_t* share_sem) {
    int num_files_processed = 0;
    char* slave_output_on_file = strtok(slave_output, SLAVE_OUTPUT_DELIM);

    while (slave_output_on_file != NULL) {
        fprintf(output_file, "slave (pid=%d): %s\n", slave_pid, slave_output_on_file);
        add_info_to_shared_data(&shared_buffer[num_files_processed + files_added], slave_output_on_file, slave_pid, share_sem);

        slave_output_on_file = strtok(NULL, SLAVE_OUTPUT_DELIM);
        num_files_processed++;
    }

    return num_files_processed;
}

void* create_shared_memory(size_t size){
    int shared_obj = shm_open(SHARED_NAME, O_CREAT | O_RDWR, 0777);
    if(shared_obj == -1){
        fprintf(stderr, "Error: could not open shared memory object\n");
        return NULL;
    }
    ftruncate(shared_obj, size);
    void * dir = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_obj, 0);
    close(shared_obj);
    return dir;
}

int execute_jobs_on_files(slave_worker* slave_workers, int num_slaves, char* files[], int num_file_args, FILE* output_file, sem_t* share_sem, shared_data* shared_buffer) {
    int write_file_index = distribute_initial_jobs_to_slaves(slave_workers, num_slaves, files, num_file_args);
    int read_file_index = 0;

    while (read_file_index < num_file_args) {
        fd_set to_read_fds;
        fd_set to_write_fds;

        FD_ZERO(&to_read_fds);
        FD_ZERO(&to_write_fds);

        int max_fd = 0;
        for (int i = 0; i < num_slaves; i++) {
            max_fd = (slave_workers[i].pipes.out[R_END] > max_fd) ? slave_workers[i].pipes.out[R_END] : max_fd;
            max_fd = (slave_workers[i].pipes.in[W_END] > max_fd) ? slave_workers[i].pipes.in[W_END] : max_fd;

            FD_SET(slave_workers[i].pipes.out[R_END], &to_read_fds);
            FD_SET(slave_workers[i].pipes.in[W_END], &to_write_fds);
        }

        select(max_fd + 1, &to_read_fds, &to_write_fds, NULL, NULL); 

        for (int i = 0; i < num_slaves; i++) {
            if (write_file_index < num_file_args && !slave_workers[i].is_processing_job)
                write_file_index += add_job_to_slave(files + write_file_index, 1, &slave_workers[i]);

            if (FD_ISSET(slave_workers[i].pipes.out[R_END], &to_read_fds)) {
                char slave_output[SLAVE_OUTPUT_MAX_LEN * slave_workers[i].num_files_in_job];
                int chars_read = read(slave_workers[i].pipes.out[R_END], slave_output, SLAVE_OUTPUT_MAX_LEN);
                slave_output[chars_read] = '\0';

                int num_files_processed = process_slave_output(slave_output, output_file, slave_workers[i].pid, shared_buffer, read_file_index, share_sem);

                slave_workers[i].num_files_processed_in_job += num_files_processed;
                if (slave_workers[i].num_files_processed_in_job == slave_workers[i].num_files_in_job)
                    slave_workers[i].is_processing_job = 0;

                read_file_index += num_files_processed;
            }
        }
    }

    return 0;
}

int close_slaves(slave_worker* slave_workers, int num_slaves) {
    for (int i = 0; i < num_slaves; i++) {
        close(slave_workers[i].pipes.in[W_END]);
        close(slave_workers[i].pipes.out[R_END]);
        int stat;
        pid_t pid = waitpid(slave_workers[i].pid, &stat, 0);
        if(pid == -1) return -1;
    }
    return 0;
}

int main(int argc, char * argv[]){
    if (argc < 2) {
        fprintf(stderr, "Error: input files not provided.\n");
        return 1;
    }

    slave_worker slave_workers[MAX_NUMBER_OF_SLAVES]; 

    int num_file_args = argc - 1;
    const int num_slaves = min(MAX_NUMBER_OF_SLAVES, div_ints_ceil(num_file_args, NUM_FILES_IN_INITIAL_JOB));

    if (create_slaves(slave_workers, num_slaves) == -1) {
        fprintf(stderr, "Error: could not create slaves");
        return 1;
    }

    FILE* output_file = fopen(OUTPUT_FILE_NAME, "w+");
    if (output_file == NULL) {
        fprintf(stderr, "Error: could not open output file\n");
        return 1;
    }

    sem_t* share_sem = sem_open(SHARED_NAME, O_CREAT, 0777, 0);

    size_t shared_buff_size = (num_file_args + 1) * sizeof(shared_data);
    shared_data* shared_buffer = create_shared_memory(shared_buff_size);
    if(shared_buffer == NULL) return -1;

    printf("%s\n", SHARED_NAME);
    fflush(stdout);

    sleep(WAIT_DURATION);

    execute_jobs_on_files(slave_workers, num_slaves, argv + 1, num_file_args, output_file, share_sem, shared_buffer);
 
    fclose(output_file);
    close_slaves(slave_workers, num_slaves);

    // prevets blocking in ./view
    sem_post(share_sem);

    sem_close(share_sem);
    sem_unlink(SHARED_NAME);

    munmap(shared_buffer, shared_buff_size);
    shm_unlink(SHARED_NAME);

    return 0;
}
