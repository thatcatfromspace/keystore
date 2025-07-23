#pragma once

class TcpClient {
  public:
    const int PORT = 8080;
	const int BUFFER_SIZE = 1024;

    int sock = 0;
	struct sockaddr_in serv_addr;

    TcpClient();
    TcpClient(int port, int buffer_size);

    int createConnection();

    std::string sendMessage(const std::string& message);

    void closeConnection();
};