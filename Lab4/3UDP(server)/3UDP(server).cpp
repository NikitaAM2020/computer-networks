#include <iostream>
#include <string>
#include <WinSock2.h>
#include <set>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")
#define INET_ADDRSTRLEN 16
#pragma warning(disable : 4996) 

using namespace std;

// Використовуємо set для зберігання активних клієнтів
set<string> activeClients;

// М'ютекс для блокування доступу до змінних, які використовуються сервером
mutex clientMutex;

const int SERVER_PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock." << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Failed to create server socket." << endl;
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Failed to bind server socket." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server is listening on port " << SERVER_PORT << endl;

    char buffer[BUFFER_SIZE];

    while (true) {
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);

        // Отримання даних від клієнта
        int bytesRead = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            string clientIP = inet_ntoa(clientAddr.sin_addr);

            // Блокування доступу до серверу, поки ми працюємо із попереднім клієнтом
            unique_lock<mutex> lock(clientMutex);

            // Перевірка, чи клієнт є активним
            if (activeClients.find(clientIP) != activeClients.end()) {
                // Клієнт вже активний, можна відхилити запит
                cout << "Ignoring broadcast from " << clientIP << ": " << buffer << endl;
            }
            else {
                // Клієнт ще не активний, можна обробити запит
                activeClients.insert(clientIP);

                // Надсилання відповіді на той самий IP-адресу та порт
                sendto(serverSocket, buffer, bytesRead, 0, (struct sockaddr*)&clientAddr, clientAddrLen);
                cout << "Received broadcast from " << clientIP << ": " << buffer << endl;

                // Позначення клієнта як активного
                activeClients.erase(clientIP);
            }
        }
    }

    // Закриття серверного сокету та очищення ресурсів Winsock
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
