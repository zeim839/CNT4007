#include "network.hpp"

// Connection implementation.

Connection::Connection(CFG_PEER peer)
{
	sockaddr_in server_addr;
	hostent* server_info;

	// Resolve hostname.
	server_info = gethostbyname(peer.hostname.c_str());
	ASSERT(server_info, "Could not resolve host %s", peer.hostname.c_str());

	// Create socket.
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	ASSERT(sockfd != 1, "could not open socket for peer %s:%d",
	       peer.hostname.c_str(), peer.port);

	// Set up server address structure.
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	bcopy((char *)server_info->h_addr, (char *)&server_addr.sin_addr.s_addr,
	      server_info->h_length);

	server_addr.sin_port = htons(peer.port);

	// Connect to server.
	int conn = connect(sockfd, (struct sockaddr*)&server_addr,
                sizeof(server_addr));

	ASSERT(conn != -1, "failed to connect to peer %s:%d",
	       peer.hostname.c_str(), peer.port);

	this->_socket = sockfd;
}

Connection::Connection(int socket)
{
	ASSERT(socket != -1, "cannot open connection for socket -1");
	this->_socket = socket;
}

Connection* Connection::accept(int socket)
{ return new Connection(socket); }

int Connection::transmit(const Byte buffer[], unsigned int len)
{
	if (this->isClosed() || len == 0)
		return 0;

	int sent_bytes = send(this->_socket, buffer, len, 0);
	if (sent_bytes == -1) this->close();

	return sent_bytes;
}

int Connection::receive(Byte buffer[], unsigned int max_len)
{
	if (this->isClosed() || max_len == 0)
		return false;

	// Wait for data to arrive.
	int received_bytes = recv(this->_socket, buffer, max_len - 1, 0);
	if (received_bytes == -1 || received_bytes == 0)
		this->close();

	// Null-terminate the received data.
	buffer[received_bytes] = '\0';
	return received_bytes + 1;
}

void Connection::close()
{
	if (this->isClosed())
		return;

	::close(this->_socket);
	this->_isClosed = true;
}

// Server implementation.

Server::Server(unsigned int port)
{
	this->_port = port;

	// Create a TCP socket.
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	ASSERT(serverSocket != -1, "could not allocate socket for server with "
	       "port %d", port);

	// Bind the socket to the port.
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	int binding = bind(serverSocket, (struct sockaddr*)&serverAddress,
                sizeof(serverAddress));

	ASSERT(binding != -1, "could not start server on port %d", port);

	// Allow incoming connections (but do not accept, yet).
	ASSERT(::listen(serverSocket, 20) != -1, "error listening for connections "
	       "for server on port %d", port);

	this->_socket = serverSocket;
}

Connection* Server::listen()
{
	ASSERT(!this->isClosed(), "cannot listen for incoming connections on "
	       "port %d: server closed", this->port());

	sockaddr_in clientAddress;
	socklen_t clientAddressSize = sizeof(clientAddress);
	int clientSocket = -1;

	// Keep trying until a good connection arrives.
	while (clientSocket == -1) {
		clientSocket = accept(this->_socket,
                        (struct sockaddr*)&clientAddress,
			&clientAddressSize);
	}

	return Connection::accept(clientSocket);
}

void Server::close()
{
	if (this->isClosed()) return;
	::close(this->_socket);
	this->_isClosed = true;
}
