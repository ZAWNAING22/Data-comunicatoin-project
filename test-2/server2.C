#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h> // POSIX Threads (Pthreads) library

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to handle client communication
void *handle_client(void *client_socket_ptr) {
    SOCKET client_socket = *(SOCKET *)client_socket_ptr;
    free(client_socket_ptr);

    char buffer[BUFFER_SIZE];
    int recv_size;

    while ((recv_size = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[recv_size] = '\0'; // Null-terminate the received data
        printf("Client: %s\n", buffer);

        // Respond based on client message
        if (strcmp(buffer, "hi") == 0) {
            strcpy(buffer, "hello");
        } else if (strcmp(buffer, "how") == 0) {
            strcpy(buffer, "fine");
        } else if (strcmp(buffer, "name") == 0) {
            strcpy(buffer, "Jack");
        } else if (strcmp(buffer, "age") == 0) {
            int random_number = (rand() % 100) + 1;
            char str[10];
            sprintf(str, "%d", random_number); // Convert integer to string
            strcpy(buffer, str);
        } else if (strcmp(buffer, "study") == 0) {
            strcpy(buffer, "computer");
        } else if (strcmp(buffer, "thanks") == 0) {
            strcpy(buffer, "thank you");
        } else {
            char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            char word[6];
            for (int i = 0; i < 5; i++) {
                int random_char = (rand() % 26); // Range: [0, 25]
                word[i] = letters[random_char];
            }
            word[5] = '\0'; // Null-terminate the string
            strcpy(buffer, word);
        }

        send(client_socket, buffer, strlen(buffer), 0);
    }

    printf("Client disconnected.\n");
    closesocket(client_socket);
    return NULL;
}

int main() {
    WSADATA wsaData;
    SOCKET server_socket;
    struct sockaddr_in server_address, client_address;
    int client_len;

    // Initialize Winsock
    //MAKEWORD(2, 2) represents version 2.2 of the Winsock API.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configure server address
    server_address.sin_family = AF_INET;// for ip6 ->AF_INET6
    server_address.sin_addr.s_addr = INADDR_ANY;// Sets the IP address for the server. for any network
    server_address.sin_port = htons(PORT); //from host byte order to network byte order.

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        printf("Bind failed. Error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed. Error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept and handle multiple clients
    while (1) {
        client_len = sizeof(client_address);
        SOCKET *client_socket = malloc(sizeof(SOCKET)); // Allocate memory for client socket
        if ((*client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len)) == INVALID_SOCKET) {
            printf("Accept failed. Error: %d\n", WSAGetLastError());
            free(client_socket);
            continue;
        }

        printf("Client connected.\n");

        // Create a new thread for the client
        pthread_t client_thread;  // Thread identifiers
        if (pthread_create(&client_thread, NULL, handle_client, client_socket) != 0) {
            printf("Failed to create thread.\n");
            closesocket(*client_socket);
            free(client_socket);
        } else {
            pthread_detach(client_thread); // Detach thread to avoid memory leaks
        }
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}
