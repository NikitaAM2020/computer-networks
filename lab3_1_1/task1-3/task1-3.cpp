#include <windows.h>
#include <iostream>
using namespace std;

int main() {
    // Open a text file and write your message to it.
    HANDLE hFile = CreateFile(
        L"example.txt",           // Name of the text file.
        GENERIC_READ | GENERIC_WRITE, // Open for read/write access.
        0,                        // Do not share.
        NULL,                     // Default security.
        CREATE_ALWAYS,            // Creates a new file, always.
        FILE_ATTRIBUTE_NORMAL,    // Normal file.
        NULL);                    // No attribute template.

    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "Could not open file: " << GetLastError() << endl;
        return 1;
    }

    const char* message = "Hello from the writer process (a message from the file)!";
    DWORD bytesWritten;

    if (!WriteFile(hFile, message, strlen(message), &bytesWritten, NULL)) {
        cerr << "Failed to write to file: " << GetLastError() << endl;
        CloseHandle(hFile);
        return 1;
    }

    cout << "Message written to file: " << message << endl;

    // Create a file mapping.
    HANDLE hMapFile = CreateFileMapping(
        hFile,                    // Use the file.
        NULL,                     // Default security.
        PAGE_READWRITE,           // Read/write access.
        0,                        // Maximum object size (high-order DWORD).
        256,                      // Maximum object size (low-order DWORD).
        L"MySharedMemory");       // Name of mapping object.

    if (hMapFile == NULL) {
        cerr << "Could not create file mapping object: " << GetLastError() << endl;
        return 1;
    }

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
    CloseHandle(hMapFile);

    return 0;
}
