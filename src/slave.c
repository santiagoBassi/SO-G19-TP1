#include "slave.h"

int main(void)
{

    char filesToProcess[BUFFER_SIZE];
    char bufferHash[BUFFER_SIZE];
    
    char * argv[3];
    argv[0] = MD5_PATH;


    int pipeFd[2];
    

    while (TRUE) {    
        
        int p1 = pipe(pipeFd);
        if(p1 == -1){
            perror("Error: Failed to create pipe.");
            //ACA QUE HACEMOS?? 
        }
        
        fgets(filesToProcess, 256, stdin);
        filesToProcess[strcspn(filesToProcess, "\n")] = 0;
        
        
        int pid = fork();
        
        if (pid != 0) {
            close(pipeFd[1]);

            waitpid(pid, NULL, 0);

            read(pipeFd[0],bufferHash, 256);
            puts(bufferHash);
            bufferHash[0]=0;
            close(pipeFd[0]);
        }
        else {
            close(pipeFd[0]);
            close(1);
            dup(pipeFd[1]);
            close(pipeFd[1]);
            //la salida estandar del hijo ahora mira al pipe asi md5 manda la salida al pipe y slave la puede leer

            
            
            //tengo que copiar el argumento para que ejecute con el file
            argv[1] = filesToProcess;
            execve(MD5_PATH, argv, NULL);
            
            return 0;
        }
       
    
    }
}