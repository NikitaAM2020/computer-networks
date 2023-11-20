#include <iostream>
#include <string>
#include <WinSock2.h>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

const int SERVER_PORT = 8080;
const int BUFFER_SIZE = 1024;

using namespace std;

int main() {
    // Ініціалізація бібліотеки Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock." << endl;
        return 1;
    }

    // Створення сокету для клієнта з використанням протоколу UDP
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Failed to create client socket." << endl;
        WSACleanup();
        return 1;
    }

    // Налаштування параметрів сервера (IP-адреса, порт) для broadcast
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_BROADCAST;

    // Увімкнення broadcast для сокету
    int enable = 1;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, (char*)&enable, sizeof(enable)) == SOCKET_ERROR) {
        cerr << "Failed to enable broadcast." << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Підготовка повідомлення для відправки на сервер
    string message = "Hello! It's the third task (UDP broadcasting).";

    // Відправлення повідомлення на вказану адресу broadcast
    sendto(clientSocket, message.c_str(), message.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    char buffer[BUFFER_SIZE];
    vector<SOCKET> servers;

    while (servers.empty()) {
        // Отримання відповіді від сервера
        int bytesRead = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            cout << "Received response: " << buffer << endl;

            // Перевірка, чи отримана відповідь є "Server is ready".
            if (string(buffer) == "Server is ready") {
                servers.push_back(clientSocket);
            }
        }
    }

    SOCKET selectedServer = servers[0]; // Вибір першого доступного сервера

    // Закриття сокету клієнта та очищення ресурсів Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
