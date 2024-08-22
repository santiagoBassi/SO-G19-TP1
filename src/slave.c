#include "slave.h"

int main(void){
    
    char buffer[256];

    while(TRUE){
        fgets(buffer, 256, stdin);
        int pid = fork();
        if(pid != 0){
            printf("Soy el padre");//tengo que acomodar los fd y llamar al
        }else{
            printf("Soy el hijo");//execev() ... 
        }
        return 0;
    }


}