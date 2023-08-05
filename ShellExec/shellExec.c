#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    printf("Loading ShellExec...\n\n");

    if(argc == 1 || argc >= 3){
        printf("Select one argument file.exe -> PID");
        return EXIT_FAILURE;
    }

    //void do something....
    shellExec(argc);

    printf("program runs correctly");
    return EXIT_SUCCESS;
}