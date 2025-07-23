#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "tcp_client.h"
#include <spdlog/spdlog.h>

TcpClient::TcpClient() {
	createConnection();
};

TcpClient::TcpClient(int port, int buffer_size) : PORT(port), BUFFER_SIZE(buffer_size) {
	createConnection();
};

int TcpClient::createConnection() {
	spdlog::set_level(spdlog::level::debug);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		spdlog::error("[TcpClient] Socket creation error");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		spdlog::error("[TcpClient] Invalid address / Address not supported");
		return -1;
	}

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		spdlog::error("[TcpClient] Connection failed");
		return -1;
	}

	spdlog::info("[TcpClient] Connected to server.");
    return 0;
}

std::string TcpClient::sendMessage(const std::string& message) {
	char buffer[BUFFER_SIZE] = {0};
	const char* message_buffer = message.c_str();
	send(sock, message_buffer, strlen(message_buffer), 0);
	spdlog::debug("[TcpClient] Message sent to server: {}", message);

    std::string response;

	long valread = read(sock, buffer, BUFFER_SIZE);
	if (valread > 0) {
		buffer[valread] = '\0';
		response = buffer;
		spdlog::debug("[TcpClient] Server response: {}", response);
	} else if (valread == 0) {
		spdlog::warn("[TcpClient] Server closed the connection");

	} else {
		spdlog::error("[TcpClient] Error reading from server");
	}
    return response;
}

void TcpClient::closeConnection() {
	close(sock);
	spdlog::info("[TcpClient] Client shut down.");
}

int main() {
    TcpClient client;
    if (client.createConnection() != 0) {
        return -1;
    }

    std::string message = "Hello from client";
    std::string response = client.sendMessage(message);
    if (!response.empty()) {
        std::cout << "Response from server: " << response << std::endl;
    }

    client.closeConnection();
    return 0;
}