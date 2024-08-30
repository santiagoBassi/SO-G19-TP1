// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <view.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>

size_t read_shared_name(char** shared_name){
    char* name = NULL;
    size_t cap = 0;
    ssize_t len = getline(&name, &cap, stdin);
    if(len == -1){
        fprintf(stderr, "Error: could not read semaphore name\n");
        return -1;
    }
    name[len-1] = '\0';
    *shared_name = name;
    return len-1;
}

void* map_shared_memory(char* shared_name){
    int shared_obj = shm_open(shared_name, O_RDWR, 0777);
    if(shared_obj == -1){
        fprintf(stderr, "Error: could not open shared memory object\n");
        return NULL;
    }
    struct stat buf;
    fstat(shared_obj, &buf);
    return mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_obj, 0);
}

void show_results(shared_data* shared_buffer, int index){
    printf("slave (pid=%d):%s  %s\n", shared_buffer[index].slave_pid, shared_buffer[index].hash, shared_buffer[index].filename);
}

int main(){

    int results_displayed = 0;

    char* shared_name;
    int shared_name_len = read_shared_name(&shared_name);
    if(shared_name_len == -1) return 1;

    sem_t* share_sem = sem_open(shared_name, O_CREAT, 0777, 0);
    shared_data* shared_buffer = map_shared_memory(shared_name);
    if(shared_buffer == NULL) return 1;
    while(sem_wait(share_sem) != -1){
        printf("reading result %d...\n", results_displayed);
        show_results(shared_buffer, results_displayed++);
    }

    return 0;
}