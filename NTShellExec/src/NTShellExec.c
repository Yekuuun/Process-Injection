#include <stdio.h>
#include <Windows.h>
#include "utils.h"

//https://www.unknowncheats.me/forum/c-and-c-/69367-lame-base-call-nt-functions.html
//https://github.com/zodiacon/ErrorLookup


int main(int argc, char* argv[]){
    printf("Loading NTShellExec.... \n\n");

    if(argc == 1 || argc >= 3){
        printf("Select one argument <file.exe> -> PID");
        return EXIT_FAILURE;
    }

    printf("size of shellcode : %zd bytes", sizeof(shellcode_x64));

    return EXIT_SUCCESS;
}