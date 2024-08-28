// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <view.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(){
    sleep(2);
    printf("Reading buffer...\n");
    int shared_obj = shm_open("/shared_buffer", O_CREAT | O_RDWR, 0777);
    if(shared_obj == -1){
        fprintf(stderr, "Error: could not open shared memory object\n");
        return -1;
    }
    struct stat buf;
    fstat(shared_obj, &buf);
    shared_data* shared_buffer = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_obj, 0);
    printf("%s\n", shared_buffer[0].filename);
    return 0;
}