#ifndef CNT4007_NETWORK_HPP
#define CNT4007_NETWORK_HPP

#include "system.hpp"
#include "config.hpp"

/*
 * Connection is an established TCP connection with a foreign
 * host/port. It does not enforce protocol rules, just manages
 * the connection.
 */
class Connection
{
public:

	/*
	 * Open a new TCP connection using the details specified in
	 * the CFG_PEER peer entry.
	 */
	Connection(CFG_PEER peer);

	// Create a connection object from a socket.
	static Connection* accept(int socket);

	/*
	 * Transmit data to the connected peer, returns whether the
	 * data was successfully transmitted. Buffer is the data to
	 * transmit, len is the length of the buffer array.
	 */
	bool transmit(const Byte buffer[], unsigned int len);

	/*
	 * Blocks until a message is received, then writes up to
	 * max_len - 1 bytes to buffer. The written buffer data is
	 * null-terminated. Returns whether data was successfully
	 * received.
	 */
	bool receive(Byte buffer[], unsigned int max_len);

	/*
	 * Close the connection. Once a server is closed, it cannot be
	 * opened again.
	 */
	void close();

	// isClosed returns whether the connection is closed.
	bool isClosed() { return this->_isClosed; }

private:
	Connection(int socket);

	// The socket number that manages the peer connection.
	int _socket;

	// Whether the connection is closed.
	bool _isClosed = false;
};

/*
 * Server is a TCP server that listens for incoming TCP connections.
 * It does not enforce protocol rules, just manages the TCP server.
 */
class Server
{
public:

	// Create a server on the designated port.
	Server(unsigned int port);

	/*
	 * Blocking function that listens until a new connection
	 * arrives. The connection is then accepted and a Connection
	 * object is dynamically allocated.
	 */
	Connection* listen();

	/*
	 * Close the server & socket. Existing connections will remain
	 * open but the server will not be reachable to new peers.
	 */
	void close();

	// port returns the port assigned to this server.
	unsigned int port() { return this->_port; }

	// isClosed returns whether the server is closed.
	bool isClosed() { return this->_isClosed; }

private:
	unsigned int _port;

	// The socket number that manages the peer connection.
	int _socket;

	/*
	 * If closed, then the server is not accepting incoming
	 * connections.
	 */
	bool _isClosed = false;
};

#endif // CNT4007_NETWORK_HPP
