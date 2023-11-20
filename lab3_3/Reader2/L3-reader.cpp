#include <windows.h>
#include <iostream>
using namespace std;

int main() {
    HANDLE hPipe;
    const wchar_t* pipeName = L"\\\\.\\pipe\\MyNamedPipe"; // Update this with the server's pipe name.

    // Connect to the named pipe.
    hPipe = CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        cerr << "Failed to connect to the server: " << GetLastError() << endl;
        return 1;
    }

    cout << "Connected to the server." << endl;

    // Receive data from the server.
    char buffer[256] = { 0 };
    DWORD bytesRead;
    if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
        cout << "Received message from server: " << buffer << endl;
    }
    else {
        cerr << "Failed to read data: " << GetLastError() << endl;
    }

    // Close the named pipe.
    CloseHandle(hPipe);

    return 0;
}
