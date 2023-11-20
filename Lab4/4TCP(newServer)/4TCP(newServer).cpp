#include <iostream>
#include <string>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

const int SERVER_PORT = 8080;
const int BUFFER_SIZE = 1024;

using namespace std;

vector<SOCKET> connectedClients;  // Зберігає сокети підключених клієнтів
mutex clientMutex;  // М'ютекс для синхронізації доступу до списку клієнтів

void HandleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytesRead] = '\0';

        // Виводимо повідомлення від клієнта
        cout << "Client says: " << buffer << endl;

        // Відправляємо відповідь назад клієнту
        send(clientSocket, buffer, bytesRead, 0);
    }

    // При завершенні з'єднання, видаляємо клієнта зі списку
    lock_guard<mutex> lock(clientMutex);
    auto it = find(connectedClients.begin(), connectedClients.end(), clientSocket);
    if (it != connectedClients.end()) {
        connectedClients.erase(it);
    }

    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock." << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
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

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Failed to listen for connections." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server is listening on port " << SERVER_PORT << endl;

    while (true) {
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket != INVALID_SOCKET) {
            // Додаємо клієнта до списку
            lock_guard<mutex> lock(clientMutex);
            connectedClients.push_back(clientSocket);

            // Створення окремого потоку для обслуговування клієнта
            thread clientThread(HandleClient, clientSocket);
            clientThread.detach();  // Розділення потоку від головного потоку
        }
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
