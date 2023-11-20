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

    // Створення сокету для сервера
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return -1;
    }

    // Налаштування параметрів сервера (IP-адреса і порт)
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Прив'язка сокету до певної IP-адреси та порту
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Error binding to port" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    // Прослуховування сокету на вхідні з'єднання
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Error listening on socket" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    cout << "Server is listening on port 8080..." << endl;

    // Безкінечний цикл для обробки вхідних з'єднань
    while (true) {
        // Прийняття з'єднання з клієнтом
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Error accepting client connection" << endl;
            closesocket(serverSocket);
            WSACleanup();
            return -1;
        }

        char buffer[1024];
        // Отримання даних від клієнта
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == SOCKET_ERROR) {
            cerr << "Error receiving data from client" << endl;
        }
        else {
            buffer[bytesRead] = '\0';
            cout << "Client connected." << endl;
            cout << "Received from client: " << buffer << endl;

            // Відправлення того самого рядка назад клієнту
            int bytesSent = send(clientSocket, buffer, bytesRead, 0);
            if (bytesSent == SOCKET_ERROR) {
                cerr << "Error sending data back to client" << endl;
            }
        }

        // Закриття сокету клієнта
        closesocket(clientSocket);
    }

    // Закриття серверного сокету та очищення ресурсів Winsock
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
