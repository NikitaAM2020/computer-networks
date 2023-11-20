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

    // Створення сокету для клієнта з використанням протоколу UDP
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return -1;
    }

    // Налаштування параметрів сервера (IP-адреса, порт)
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Порт сервера
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        cerr << "Invalid IP address" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    // Підготовка повідомлення для відправки на сервер
    const char* message = "Hello! It's the second task (UDP).";

    // Відправлення даних на сервер
    int bytesSent = sendto(clientSocket, message, static_cast<int>(strlen(message)), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bytesSent == SOCKET_ERROR) {
        cerr << "Error sending data to server" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    char buffer[1024];
    int serverAddrLen = sizeof(serverAddr);
    // Отримання даних від сервера
    int bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddr, &serverAddrLen);
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
