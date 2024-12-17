#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> // Includes Winsock API definitions
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") // Links Winsock library

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr); // Server running locally

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        printf("Connection failed. Error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("client 2-> :Connected to the server.\n");
    printf("enter->:hi,how,name,age,study,thanks or exit to leave .\n");

    // Communicate with the server
    while (1) {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Remove newline character from input if present
        buffer[strcspn(buffer, "\n")] = '\0';

        // Send message to the server
        if (send(client_socket, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            printf("Send failed. Error: %d\n", WSAGetLastError());
            break;
        }

        // Exit if the client wants to quit
        if (strcmp(buffer, "exit") == 0) {
            printf("Disconnecting from the server.\n");
            break;
        }

        // Clear the buffer and receive server response
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Null-terminate the received data
            printf("Server: %s\n", buffer);
        } else if (bytes_received == 0) {
            printf("Server disconnected.\n");
            break;
        } else {
            printf("Recv failed. Error: %d\n", WSAGetLastError());
            break;
        }
    }

    // Close socket
    closesocket(client_socket);
    WSACleanup();
    return 0;
}
