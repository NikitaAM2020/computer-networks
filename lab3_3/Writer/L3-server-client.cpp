#include <windows.h>
#include <iostream>
#include <thread>
using namespace std;

void handleClient(HANDLE hPipe) {
    cout << "Client connected." << endl;

    // Send a message to the client.
    const char* message = "Hello from the server process!";
    DWORD bytesWritten;
    BOOL isWritten = WriteFile(hPipe, message, strlen(message), &bytesWritten, NULL);

    if (isWritten) {
        cout << "Message sent to client: " << message << endl;
    }
    else {
        cerr << "Failed to send message: " << GetLastError() << endl;
    }

    // Disconnect and close the named pipe.
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}

int main() {
    while (true) {
        // Create a named pipe.
        HANDLE hPipe = CreateNamedPipe(
            L"\\\\.\\pipe\\MyNamedPipe", // Pipe name.
            PIPE_ACCESS_DUPLEX,         // Read/Write access.
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, // Byte-type pipe, blocking mode.
            PIPE_UNLIMITED_INSTANCES,   // Number of instances.
            0,                          // Output buffer size.
            0,                          // Input buffer size.
            NMPWAIT_USE_DEFAULT_WAIT,   // Time-out interval.
            NULL);                      // Default security attributes.

        if (hPipe == INVALID_HANDLE_VALUE) {
            cerr << "CreateNamedPipe failed: " << GetLastError() << endl;
            return 1;
        }

        cout << "Waiting for client connection..." << endl;

        // Wait for a client to connect.
        BOOL isConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (isConnected) {
            thread clientThread(handleClient, hPipe);
            clientThread.detach();
        }
        else {
            cerr << "Failed to connect: " << GetLastError() << endl;
            CloseHandle(hPipe);
        }
    }

    return 0;
}
