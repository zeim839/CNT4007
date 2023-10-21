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

#define MAX_NEIGHBORS 3 // idk the final value
#define BUFFER_SIZE 1024 // idk the final value

/*
 * Takes a int for the port number and opens a connection
 * tcp socket
 */
int EstablishServerSocket(int port);

/*
 * Opens a tcp client connection to a server hostname and port
 */
int EstablishClientSocket(char* hostname, int port);

/*
 * server socket accepts client connects and opens threads for
 * each connection on connectionHandler.
 */
void RecieveConnections(int serverSocket);

/*
 * handles the tcp connection, somewhat of a placeholder until
 * protocol filled out.
 */
void ConnectionHandler(int tcp_socket);

#endif
