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

    // Створення сокету для сервера з використанням протоколу UDP
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return -1;
    }

    // Налаштування параметрів сервера (IP-адреса, порт)
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);  // Порт сервера
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Сервер слухає на будь-якій IP-адресі

    // Прив'язка сокету до певного порту та адреси
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Error binding to port" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    cout << "Server is listening on port 8080..." << endl;

    while (true) {
        sockaddr_in clientAddr; // Структура для зберігання інформації про клієнта
        int clientAddrLen = sizeof(clientAddr);
        char buffer[1024];
        // Отримання даних від клієнта
        int bytesRead = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (bytesRead == SOCKET_ERROR) {
            cerr << "Error receiving data from client" << endl;
        }
        else {
            buffer[bytesRead] = '\0';
            cout << "Client connected." << endl;
            cout << "Received from client: " << buffer << endl;

            // Відправлення того самого рядка назад клієнту
            int bytesSent = sendto(serverSocket, buffer, bytesRead, 0, (struct sockaddr*)&clientAddr, clientAddrLen);
            if (bytesSent == SOCKET_ERROR) {
                cerr << "Error sending data back to client" << endl;
            }
        }
    }

    // Закриття серверного сокету та очищення ресурсів Winsock
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
