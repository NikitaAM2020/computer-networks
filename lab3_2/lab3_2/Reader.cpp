#include <iostream>
#include <windows.h>

int main() {
    // Відкриття іменованого каналу для читання (клієнтський канал)
    HANDLE hPipeClient = CreateFile(
        L"\\\\.\\pipe\\MyNamedPipe", // Ім'я іменованого каналу
        GENERIC_READ,               // Доступ для читання
        0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipeClient == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening named pipe: " << GetLastError() << std::endl;
        return 1;
    }

    // Читання рядка з іменованого каналу
    char buffer[256];
    DWORD bytesRead;
    if (ReadFile(hPipeClient, buffer, sizeof(buffer), &bytesRead, NULL)) {
        buffer[bytesRead] = '\0'; // Додавання завершуючого нуля
        std::cout << "Message from the server: " << buffer << std::endl;
    }
    else {
        std::cerr << "Error reading data from the named pipe: " << GetLastError() << std::endl;
    }

    CloseHandle(hPipeClient);

    return 0;
}
