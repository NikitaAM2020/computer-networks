#include <windows.h>
#include <iostream>
using namespace std;

int main() {
    DWORD pid = GetCurrentProcessId();

    // Open the existing file mapping.
    HANDLE hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,     // Read/write permission.
        FALSE,                   // Do not inherit the name.
        L"MySharedMemory");      // Name of mapping object.

    if (hMapFile == NULL) {
        cerr << "Could not open file mapping object: " << GetLastError() << endl;
        return 1;
    }


    // Map the view of the file into the address space of the process.
    char* pBuf = (char*)MapViewOfFile(
        hMapFile,                // Handle to map object.
        FILE_MAP_ALL_ACCESS,     // Read/write permission.
        0,
        0,
        256);

    if (pBuf == NULL) {
        cerr << "Could not map view of file: " << GetLastError() << endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // Read from the shared memory.
    printf("PROC.%X: Message read from shared memory: %s", pid, pBuf);

    // Clean up.
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}
