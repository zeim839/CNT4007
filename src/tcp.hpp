#ifndef CNT4007_TCP_HPP
#define CNT4007_TCP_HPP

#include <sys/socket.h> 
#include <unistd.h>
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <netdb.h>
#include <cstring>
#include <mutex>
#include <map>

#define MAX_NEIGHBORS 3 // idk the final value
#define BUFFER_SIZE 1024 // idk the final value

#include "tcp.hpp"

class TCP
{
    private:
    std::mutex bitfieldMutex;
    std::vector<char*> bitfields; //placeholder until u know what kind of type bitfield is
    std::vector<std::thread> connections;
    std::thread serverReciver;
    char* port;
    char* hostname;
    int serverSocket;
    int clientSocket;
    bool serverListening;

    /*
     * Takes a int for the port number and opens a connection
     * tcp socket
     */
    int EstablishServerSocket();

    /*
     * adds a connection ot the connections vector to be managed by object
     * spins up a thread for each one to ConnectionHandler
     */
    void addConnection(int connectionSocket);

    /*
     * server socket accepts client connects and opens threads for
     * each connection on connectionHandler.
     */
    void RecieveConnections();

    /*
     * Opens a tcp client connection to a server hostname and port
     */
    int EstablishClientSocket(char* connectionHostname, char* connectionPort);

    /*
     * handles the tcp connection, somewhat of a placeholder until
     * protocol filled out.
     * possibly used by both client and server relationship symetric?
     */
    void ConnectionHandler(int tcpSocket, int connectionNum);

    /*
     * when a server socket connects function adds bitfield to its
     * bitfield vector.
     */
    void addBitfield(int connectionNum, char* bitfield);


    public:
    /*
     * starts the tcp connection handler, hostname and port are for server socket
     */
    TCP(char* hostname, char* port);

    /*
     * reaches out to a hostname and port and attempts a connection
     */
    bool connectToServerSocket(char* hostname, char* port);

    /*
     * possible function to compare bitfields
     */
    void readBitfields();
};

void ConnectionHandler(int tcp_socket);

#endif
