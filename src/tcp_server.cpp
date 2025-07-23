#include "tcp_server.h"
#include <cstring>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <unistd.h>

TcpServer::TcpServer() : PORT(8080), BUFFER_SIZE(1024), addrlen(sizeof(address)) {
}

TcpServer::TcpServer(int port, int buffer_size) : PORT(port), BUFFER_SIZE(buffer_size), addrlen(sizeof(address)) {
}

int TcpServer::start() {
	spdlog::set_level(spdlog::level::debug);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		spdlog::error("[TcpServer] Socket failed");
		return -1;
	}

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		spdlog::error("[TcpServer] Setsockopt failed");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		spdlog::error("[TcpServer] Bind failed");
		return -1;
	}

	if (listen(server_fd, 3) < 0) {
		spdlog::error("[TcpServer] Listen failed");
		return -1;
	}
	spdlog::info("[TcpServer] Server listening on port {}", PORT);

	if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
		spdlog::error("[TcpServer] Accept failed");
		return -1;
	}
	spdlog::info("[TcpServer] Received client connection {}", client_socket);
	return 0;
}

std::string TcpServer::receiveMessage() {
	char buffer[BUFFER_SIZE] = {0};
	long valread = read(client_socket, buffer, BUFFER_SIZE);
	if (valread > 0) {
		buffer[valread] = '\0';
        std::string request(buffer);
		spdlog::debug("[TcpServer] Client message: {}", request);
		return request;
	} else if (valread == 0) {
		spdlog::warn("[TcpServer] Client closed the connection");
		return "";
	} else {
		spdlog::error("[TcpServer] Error reading from client");
		return "";
	}
}

void TcpServer::sendMessage(const std::string& message) {
	send(client_socket, message.c_str(), message.size(), 0);
	spdlog::debug("[TcpServer] Message sent to client: {}", message);
}

void TcpServer::closeConnection() {
	close(client_socket);
	close(server_fd);
	spdlog::info("[TcpServer] Server shut down.");
}
