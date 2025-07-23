#pragma once
#include <netinet/in.h>
#include <string>

class TcpServer {
  public:
	const int PORT;
	const int BUFFER_SIZE;
	int server_fd = 0;
	int client_socket = 0;
	struct sockaddr_in address;
	int addrlen;

	TcpServer();
	TcpServer(int port, int buffer_size);
	int start();
	std::string receiveMessage();
	void sendMessage(const std::string& message);
	void closeConnection();
};
