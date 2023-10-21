#include "tcp.hpp"

int TCP::EstablishServerSocket()
{
	// server address config
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(this->port));
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

void TCP::addConnection(int connectionSocket)
{
    //adds another spot to bitfield vector
    bitfieldMutex.lock();
    bitfields.resize(bitfields.size() + 1);
	bitfieldMutex.unlock();

	socketMutex.lock();
	socketIndex.push_back(connectionSocket);
	socketMutex.unlock();
    //pushes connection into vector should always stay alighed with bitfield
    connectionThreads.push_back(std::move(
        std::thread(ConnectionHandler, connectionSocket, connectionThreads.size())));
    
}

void TCP::RecieveConnections()
{
	while(this->serverListening)
    {
		// listens for incoming connections
		if (listen(this->serverSocket, MAX_NEIGHBORS) == -1) {
		perror("listening failure");
		}
		// accepts connections moves through backlog
		int connectedServerSocket = accept(this->serverSocket, nullptr, nullptr);
		while (connectedServerSocket != -1)
		{
			// accepts connection and pushes thread onto vector
            addConnection(connectedServerSocket);
			connectedServerSocket = accept(this->serverSocket, nullptr, nullptr);
		}
	}
}

int TCP::EstablishClientSocket(char* connectionHostname, char* connectionPort)
{
	// dns lookup set up
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//dns lookup
	struct addrinfo* result;
	if (!getaddrinfo(connectionHostname, connectionPort, &hints, &result)) {
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

// possibly used by both client and server relationship symetric?
void TCP::ConnectionHandler(int tcpSocket, int connectionNum)
{
    // protocol goes in here
	// send and recieve may be threaded in the future
	char buff[BUFFER_SIZE] = {0};
	recv(tcpSocket, buff, sizeof(buff), 0);
	send(tcpSocket, buff, sizeof(buff), 0);

	//if (!validateHandshake(Handshake())) {
    //    return;
	//} // placeholder

	char* peer = buff + 27;
	addPeer(peer, connectionNum);

    recv(tcpSocket, buff, sizeof(buff), 0);
	send(tcpSocket, buff, sizeof(buff), 0);
	
	char type[1];
	memcpy(buff + 3, type, 1);
	if (atoi(type) != 5)
	{
		perror("second message not bitfield");
		//do something
	}

    addBitfield(connectionNum, buff + 4); // placeholder needs to accept larger messages
}

void TCP::addPeer(char* peer, int connectionIndex)
{
	indexMutex.lock();
	connectionIndex[peer] = connectionIndex;
	indexMutex.unlock();
}


void TCP::addBitfield(int connectionNum, char* bitfield)
{
    bitfieldMutex.lock();
    strcpy(this->bitfields.at(connectionNum), bitfield); // placeholder to copy bitfield to array
    bitfieldMutex.unlock();
}

TCP::TCP(char* hostname, char* port)
{
    this->hostname = hostname;
    this->port = port;
    this->serverListening = true;
    this->serverSocket = EstablishServerSocket();
    this->serverReciver = std::thread(RecieveConnections);
}

bool TCP::connectToServerSocket(char* hostname, char* port)
{
    clientSocket = EstablishClientSocket(hostname, port);
    if (clientSocket == -1)
        return false;
    addConnection(clientSocket); // should work?
    return true;
}

bool TCP::closeConnection(char* peerID)
{
	return closeConnection(getConnectionIndex(peerID));
}
bool TCP::closeConnection(int connectionIndex)
{
	try {
		close(socketIndex.at(connectionIndex));
		//must add more to shift indexes? necessary?
		return true;
	} catch(const std::exception& e) {
		perror(e.what());
		return false;
	}
}

char* TCP::getBitfield(char* destination, int connectionIndex)
{
    bitfieldMutex.lock();
    strcpy(destination, bitfields.at(connectionIndex));
    bitfieldMutex.unlock();
	return destination;
}

int TCP::getConnectionIndex(char* peerID)
{
	int tempIndex = -1;
    indexMutex.lock();
    tempIndex = connectionIndex[peerID];
    indexMutex.unlock();
	return tempIndex;
}
