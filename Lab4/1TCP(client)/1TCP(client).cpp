#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    // Ініціалізація бібліотеки Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock" << endl;
        return -1;
    }

    // Створення сокету для клієнта
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return -1;
    }

    // Налаштування параметрів сервера (IP-адреса і порт)
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);

    const char* serverIP = "127.0.0.1"; // IP-адреса сервера
    if (inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) <= 0) {
        cerr << "Invalid IP address" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    // Підключення клієнта до сервера
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Error connecting to server" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    const char* message = "Hello! It's the first task (TCP)."; // Повідомлення для відправки на сервер
    // Відправлення даних серверу
    int bytesSent = send(clientSocket, message, static_cast<int>(strlen(message)), 0);
    if (bytesSent == SOCKET_ERROR) {
        cerr << "Error sending data to server" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    char buffer[1024];
    // Отримання даних від сервера
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == SOCKET_ERROR) {
        cerr << "Error receiving data from server" << endl;
    }
    else {
        buffer[bytesRead] = '\0';
        cout << "Message sent to the server." << endl;
        cout << "Received from server: " << buffer << endl;
    }

    // Закриття сокету клієнта та очищення ресурсів Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
