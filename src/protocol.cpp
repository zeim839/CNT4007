#include "protocol.hpp"

bool ValidateHandshake(MSG_HANDSHAKE hs)
{
	std::string header(hs.header, 18);
	if (header != HANDSHAKE_HEADER)
		return false;

	for (int i = 0; i < hs.zeros; i++) {
		if (!hs.zeros[i])
			continue;
		return false;
	}

	return true;
}

int EstablishServerSocket(int port)
{
	// server address config
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	// AF_INET = ipv4, SOCK_STREAM = tcp, 0 = ip
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		perror("socket failure");
		close(serverSocket);
		return -1;
	}
	
	// connects the socket to the address
	if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) == -1) {
		perror("bind failure");
		close(serverSocket);
		return -1;
	}
	return serverSocket;
}

void RecieveConnections(int serverSocket)
{
	std::vector<std::thread> connections;
	while(true) {

		// listens for incoming connections
		if (listen(serverSocket, MAX_NEIGHBORS) == -1) {
		perror("listening failure");
		}

		// accepts connections moves through backlog
		int clientSocket = accept(serverSocket, nullptr, nullptr);
		while (clientSocket != -1)
		{
			// accepts connection and pushes thread onto vector
			std::thread clientConnection(ConnectionHandler, clientSocket);
			connections.push_back(std::move(clientConnection));
			clientSocket = accept(serverSocket, nullptr, nullptr);
		}
	}
}

int EstablishClientSocket(char* hostname, char* port)
{
	// dns lookup set up
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	//dns lookup
	struct addrinfo* result;
	if (!getaddrinfo(hostname, port, &hints, &result)) {
		perror("lookup failure");
		return -1;
	}

	int clientSocket = -1;
	//trying ips
	for (struct addrinfo* rp = result; rp != nullptr; rp = rp->ai_next) {
		clientSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (clientSocket == -1)
			continue;

		//connected
		if (connect(clientSocket, rp->ai_addr, rp->ai_addrlen) != -1)
			break;
		
		close(clientSocket);
	}

	freeaddrinfo(result);

	if (clientSocket == -1) {
		perror("establish connection failure");
		return -1;
	}

	return clientSocket;
}

//possibly used by both client and server relationship symetric?
void ConnectionHandler(int socket)
{
	//send and recieve may be threaded in the future
	char buff[BUFFER_SIZE] = {0};
	recv(socket, buff, sizeof(buff), 0);
	send(socket, buff, sizeof(buff), 0);

	if (!ValidateHandshake(Handshake())) {
		return;
	} //placeholder
}

MSG_HANDSHAKE Handshake(int socket) //Placeholder
{
	MSG_HANDSHAKE msg;
	std::memset(&msg.header, 0, sizeof(msg.header));
	std::memset(&msg.zeros, 0, sizeof(msg.zeros));
	std::memset(&msg.peerID, 0, sizeof(msg.peerID));
	return msg;
}
