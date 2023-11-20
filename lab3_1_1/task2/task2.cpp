#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <Winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0); // Use SOCK_DGRAM for UDP
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create client socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345); // Use the same port as the server
    serverAddress.sin_addr.s_addr = inet_addr("192.168.126.7"); // Replace with the server's IP address

    std::string message = "Hello, Server!";
    int bytesSent = sendto(clientSocket, message.c_str(), static_cast<int>(message.size()), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Send failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Sent to the server: " << message << std::endl;

    char buffer[1024] = { 0 };
    sockaddr_in fromAddress;
    int fromAddressLength = sizeof(fromAddress);

    int bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&fromAddress, &fromAddressLength);

    if (bytesRead == SOCKET_ERROR) {
        std::cerr << "Receive failed" << std::endl;
    }
    else {
        std::cout << "Received from " << inet_ntoa(fromAddress.sin_addr) << ":" << ntohs(fromAddress.sin_port) << ": " << buffer << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
