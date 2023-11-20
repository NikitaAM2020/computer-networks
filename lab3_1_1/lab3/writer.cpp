#include <windows.h>
#include <iostream>
using namespace std;

int main() {
    // Create a file mapping.
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // Use paging file.
        NULL,                    // Default security.
        PAGE_READWRITE,          // Read/write access.
        0,                       // Maximum object size (high-order DWORD).
        256,                     // Maximum object size (low-order DWORD).
        L"MySharedMemory");      // Name of mapping object.

    if (hMapFile == NULL) {
        cerr << "Could not create file mapping object: " << GetLastError() << endl;
        return 1;
    }


    // Отримання вказівника на мапований об'єкт.
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

    // Запис у спільну пам'ять.
    const char* message = "Hello from the writer process!";
    CopyMemory((PVOID)pBuf, message, strlen(message));

    cout << "Message written to shared memory: " << message << endl;

    DWORD pid = GetCurrentProcessId();
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    wchar_t cmdLn[255] = L"Reader.exe";
    if (CreateProcess(NULL, cmdLn, NULL, NULL, TRUE, FALSE, NULL, NULL, &si, &pi))
    {
        printf("PROC.%X: New process (%S - %X) started.\n", pid, cmdLn, pi.dwProcessId);
    }
    else
        printf("PROC.%X: Failed to start a new process (Reader).\n", pid);

    // Wait for the reader process to finish reading.
    cout << "\nWait for the reader process to finish reading and press any key to exit...\n" << endl;
    cin.get();

    // Clean up.
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}
