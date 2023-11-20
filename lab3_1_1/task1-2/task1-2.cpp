#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
using namespace std;

class SharedMemory {
public:
    SharedMemory(const wchar_t* name, size_t size) : m_size(size) {
        m_hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,    // Use paging file.
            NULL,                    // Default security.
            PAGE_READWRITE,          // Read/write access.
            0,                       // Maximum object size (high-order DWORD).
            m_size,                  // Maximum object size (low-order DWORD).
            name);                   // Name of mapping object.

        if (m_hMapFile == NULL) {
            cerr << "Could not create file mapping object: " << GetLastError() << endl;
            return;
        }

        m_pBuf = (char*)MapViewOfFile(
            m_hMapFile,                // Handle to map object.
            FILE_MAP_ALL_ACCESS,     // Read/write permission.
            0,
            0,
            m_size);

        if (m_pBuf == NULL) {
            cerr << "Could not map view of file: " << GetLastError() << endl;
            CloseHandle(m_hMapFile);
        }
    }

    ~SharedMemory() {
        UnmapViewOfFile(m_pBuf);
        CloseHandle(m_hMapFile);
    }

    void writeToSharedMemory(const char* message) {
        DWORD pid = GetCurrentProcessId();
        // Write to the shared memory.
        CopyMemory((PVOID)m_pBuf, message, strlen(message));

        printf("PROC.%X: Message written to shared memory: %s\n", pid, message);
    }

    void readFromSharedMemory() {
        DWORD pid = GetCurrentProcessId();

        // Read from the shared memory.
        printf("PROC.%X: Message read from shared memory: %s", pid, m_pBuf);
    }

private:
    HANDLE m_hMapFile;
    char* m_pBuf;
    size_t m_size;
};

int main() {
    DWORD pid = GetCurrentProcessId();
    SharedMemory sharedMemory(L"MySharedMemory", 256);
    char message[50];
    sprintf(message, "Hello from PROC.%X!", pid);
    sharedMemory.writeToSharedMemory(message);
    sharedMemory.readFromSharedMemory();
    return 0;
}

