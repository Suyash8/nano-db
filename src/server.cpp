#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

// Platform-specific includes
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// Helper to close socket on Windows
#define CLOSE_SOCKET(s) closesocket(s)

// Check if socket is valid
#define IS_VALID_SOCKET(s) (s != INVALID_SOCKET)
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// Helper to close socket on Linux/Mac
#define CLOSE_SOCKET(s) close(s)

// Check if socket is valid
#define IS_VALID_SOCKET(s) (s >= 0)

// Map Windows types to standard types
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#include "nanodb/command_dispatcher.hpp"
#include "nanodb/nanodb.hpp"

SOCKET setupServer(int port) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        exit(1);
    }
#endif

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (!IS_VALID_SOCKET(server_fd)) {
        std::cerr << "Socket creation failed: " << (IS_VALID_SOCKET(server_fd) ? 0 : errno) << std::endl;
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    // 4. Bind
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on 0.0.0.0 (All interfaces)
    address.sin_port = htons(port);       // Host to Network Short

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        CLOSE_SOCKET(server_fd);
        exit(1);
    }

    // 5. Listen
    // Backlog of 3 connections
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        CLOSE_SOCKET(server_fd);
        exit(1);
    }

    std::cout << "Server listening on port " << port << "..." << std::endl;
    return server_fd;
}

void handleClient(SOCKET client_socket, struct sockaddr_in client_address, CommandDispatcher& dispatcher) {
    while (true) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_read <= 0) break;

        std::string request(buffer, bytes_read);
        std::string response = dispatcher.dispatch(request);
        response += "\n";

        send(client_socket, response.c_str(), response.size(), 0);
    }

    CLOSE_SOCKET(client_socket);

    std::cout << "Connection closed from " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << std::endl;
}

int main() {
    NanoDB db;
    CommandDispatcher dispatcher(db);

    SOCKET server_socket = setupServer(8080);

    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);

        std::cout << "Waiting for connection..." << std::endl;

        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);

        if (!IS_VALID_SOCKET(client_socket)) {
            std::cerr << "Accept failed" << std::endl;
            continue; // Try accepting the next connection
        }

        std::cout << "Connection accepted from " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << std::endl;

        std::thread client_thread(handleClient, client_socket, client_address, std::ref(dispatcher));
        client_thread.detach();
    }

    // Cleanup
    CLOSE_SOCKET(server_socket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}