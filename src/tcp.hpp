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

// manages tcp connections
class TCP
{
    //will turn into struct eventually
    std::mutex bitfieldMutex;
    std::mutex indexMutex;
    std::mutex socketMutex;
    std::vector<char*> bitfields; // placeholder until u know what kind of type bitfield is
    std::vector<std::thread> connectionThreads;
    std::vector<int> socketIndex;
    std::map<const char*, int, cmp_char> connectionIndex;
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
     * adds connection index to a peerid map to be able to reference
     * bitmaps and threads from id
     */
    void addPeer(char* peer, int connectionIndex);

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
     * closes socket doesnt yet reindex the vectors, may switch to
     * map with struct holding all data much simpler but concentrates shared
     * resources maybe slower?
     */
    bool closeConnection(char* peerID);
    bool closeConnection(int connectionIndex);

    /*
     * copies bitfield into the destination array
     */
    char* getBitfield(char* destination, int connectionIndex);

    /*
     * takes peer id and returns connection index
     */
    int getConnectionIndex(char* peerID);
};

struct cmp_char
{
    bool operator()(char const *a, char const *b) const
    {
        return std::strcmp(a,b) < 0;
    }
};

#endif
