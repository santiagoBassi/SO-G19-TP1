// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <view.h>

int read_shared_name(char* shared_name){
    if (fgets(shared_name, SHARED_NAME_MAX_LEN, stdin) == NULL) {
        fprintf(stderr, "Error: could not read semaphore name\n");
        return -1;
    }

    int len = strlen(shared_name);
    if(len == 0){
        fprintf(stderr, "Error: could not read semaphore name\n");
        return -1;
    }
    
    shared_name[len-1] = '\0';
    return len-1;
}

void* map_shared_memory(char* shared_name){
    int shared_obj = shm_open(shared_name, O_RDWR, 0777);
    if (shared_obj == -1) {
        fprintf(stderr, "Error: could not open shared memory object\n");
        return NULL;
    }
    struct stat buf;
    fstat(shared_obj, &buf);
    void * dir = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_obj, 0);
    close(shared_obj);
    return dir;
}

int show_results(shared_data* shared_buffer, int index){
    if (shared_buffer[index].slave_pid == 0) 
        return 0;

    printf("slave (pid=%d): %s  %s\n", shared_buffer[index].slave_pid, shared_buffer[index].hash, shared_buffer[index].filename);
    return 1;
}

int main(int argc, char * argv[]) {
    int results_displayed = 0;

    char shared_name[SHARED_NAME_MAX_LEN];
    
    if(argc == 2){
        strncpy(shared_name, argv[1], SHARED_NAME_MAX_LEN);
    }else{
        int shared_name_len = read_shared_name(shared_name);
        if(shared_name_len == -1) return -1;
    }
    

    sem_t* share_sem = sem_open(shared_name, O_CREAT, 0777, 0);
    shared_data* shared_buffer = map_shared_memory(shared_name);

    if(shared_buffer == NULL) {
        sem_close(share_sem);
        return -1;
    }

    int data_incoming = 1;
    while (data_incoming) {
        sem_wait(share_sem);
        data_incoming = show_results(shared_buffer, results_displayed++);
    }

    sem_close(share_sem);
    sem_unlink(shared_name);

    return 0;
}
