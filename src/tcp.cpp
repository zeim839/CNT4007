#include "tcp.hpp"

class TCP
{
    std::mutex bitfieldMutex;
    std::vector<char*> bitfields; //placeholder until u know what kind of type bitfield is
    std::vector<std::thread> connections;
    std::thread serverReciver;
    char* port;
    char* hostname;
    int serverSocket;
    int clientSocket;
    bool serverListening;
    
    int EstablishServerSocket()
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

    void RecieveConnections()
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
                bitfields.resize(bitfields.size());
    			std::thread clientConnection(ConnectionHandler, connectedServerSocket, connections.size());
    			connections.push_back(std::move(clientConnection));
    			connectedServerSocket = accept(this->serverSocket, nullptr, nullptr);
    		}
    	}
    }

    int EstablishClientSocket(char* connectionHostname, char* connectionPort)
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
    void ConnectionHandler(int tcpSocket, int connectionNum)
    {
    	// send and recieve may be threaded in the future
    	char buff[BUFFER_SIZE] = {0};
    	recv(tcpSocket, buff, sizeof(buff), 0);
    	send(tcpSocket, buff, sizeof(buff), 0);

    	if (!validateHandshake(Handshake())) {
            return;
    	} // placeholder

        recv(tcpSocket, buff, sizeof(buff), 0);
    	send(tcpSocket, buff, sizeof(buff), 0);

        addBitfield(connectionNum, buff); // placeholder
    }

    void addBitfield(int connectionNum, char* bitfield)
    {
        bitfieldMutex.lock();
        this->bitfields.at(connectionNum) = atoi(bitfield); // placeholder to copy bitfield to array
        bitfieldMutex.unlock();
    }

    void readBitfields()
    {
        bitfieldMutex.lock();
        //placeholder
        bitfieldMutex.unlock();
    }

    public:

    TCP(char* hostname, char* port)
    {
        this->hostname = hostname;
        this->port = port;
        this->serverListening = true;
        this->serverSocket = EstablishServerSocket();
        this->serverReciver = std::thread(RecieveConnections);
        this->clientSocket = EstablishClientSocket();
    }
}


MSG_HANDSHAKE Handshake(int tcp_socket) //Placeholder
{
	MSG_HANDSHAKE msg;
	std::memset(&msg.header, 0, sizeof(msg.header));
	std::memset(&msg.zeros, 0, sizeof(msg.zeros));
	std::memset(&msg.peerID, 0, sizeof(msg.peerID));
	return msg;
}
