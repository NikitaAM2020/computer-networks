#include <iostream>
#include <windows.h>

int main() {
    // Створення іменованого каналу для запису (серверний канал)
    HANDLE hPipeServer = CreateNamedPipe(
        L"\\\\.\\pipe\\MyNamedPipe", // Ім'я іменованого каналу
        PIPE_ACCESS_OUTBOUND,       // Доступ для запису
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,                          // Лічильник інстансів
        0, 0, 0, NULL);

    if (hPipeServer == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateNamedPipe failed: " << GetLastError() << std::endl;
        return 1;
    }

    // Очікування підключення клієнта
    std::cout << "Waiting for client connection..." << std::endl;
    if (ConnectNamedPipe(hPipeServer, NULL)) {
        std::cout << "Client connected." << std::endl;

        // Рядок для відправлення клієнту
        std::string message = "Hello from the server process!";

        // Відправка рядка клієнту
        DWORD bytesWritten;
        if (WriteFile(hPipeServer, message.c_str(), static_cast<DWORD>(message.length()), &bytesWritten, NULL)) {
            std::cout << "Message sent to client: " << message << std::endl;
        }
        else {
            std::cerr << "Failed to send message: " << GetLastError() << std::endl;
        }

        DisconnectNamedPipe(hPipeServer);
    }
    else {
        std::cerr << "Failed to connect: " << GetLastError() << std::endl;
    }

    CloseHandle(hPipeServer);

    return 0;
}
