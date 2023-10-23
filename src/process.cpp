#include "process.hpp"

PeerProcess::PeerProcess
(unsigned int id, unsigned int port, CFG_COMMON config,
 std::vector<CFG_PEER> peers) : Server(port), _config(config), _id(id)
{
	// Start listener thread, begin accepting peer connections.
	this->thListener = std::thread(&PeerProcess::listener, this);

	// Start dialing thread, attempt to connect to peers.
	this->thDialer = std::thread(&PeerProcess::dialer, this, peers);
}

void PeerProcess::terminate()
{
	// TODO: calling .join() waits for the threads to finish, it
	// does not kill them. When terminate() is called, listener
	// will not stop because it'll hang on the Server::listen()
	// function.
	//
	// Need a way to signal threads to stop for graceful
	// shutdown.

	this->_isClosed = true;

	// Stop listener thread.
	this->thListener.join();

	// Stop dialing thread.
	this->thDialer.join();

	this->Server::close();
}

bool PeerProcess::establishPeer(Connection* conn)
{
	if (!conn) return false;

	if (conn->isClosed()) {
		delete conn;
		return false;
	}

	// Send handshake message.
	MSG_HANDSHAKE hs;
	strcpy((char*)hs.header, HANDSHAKE_HEADER);
	bzero((char*)hs.zeros, 10);
	hs.peerID = this->id();

	Byte* buffer = hs.bytes();
	if (!conn->transmit(buffer, 32)) {
		free(buffer);
		conn->close();
		delete conn;
		return false;
	}
	free(buffer);

	// Wait for a response.

	// 32 bytes for data, 1 byte for null terminator.
	Byte inBuffer[33];
	if (!conn->receive(inBuffer, 33)) {
		conn->close();
		delete conn;
		return false;
	}

	MSG_HANDSHAKE inHs = MSG_HANDSHAKE::fromBytes(inBuffer);
	if (!validateHandshake(inHs)) {
		conn->close();
		delete conn;
		return false;
	}

	// Ensure we don't already have this peer ID.
	if (this->conns.find(inHs.peerID) == this->conns.end()) {
		conn->close();
		delete conn;
		return false;
	}

	// TODO: Exchange bitfields.

	// Add peer to connections map.
	this->mx.lock();
	this->conns[inHs.peerID] = conn;
	this->mx.unlock();

	return true;
}

void PeerProcess::dialer(std::vector<CFG_PEER> peers)
{
	for (auto it = peers.begin(); it != peers.end(); ++it) {
		// Establish TCP connection.
		Connection* conn = NULL;
		try { conn = new Connection(*it); }
		catch (...) { continue; }

		// Establish peer asynchronously.
		std::thread(&PeerProcess::establishPeer, this, conn).detach();
	}
}

void PeerProcess::listener()
{
	while (!this->isClosed()) {
		Connection* conn = this->Server::listen();
		std::cout << "incoming connection" << std::endl;

		// Establish peer asynchronously.
		std::thread(&PeerProcess::establishPeer, this, conn).detach();
	}
}

void PeerProcess::receiver(unsigned int id, Connection* conn)
{
	// TODO: listen for messages coming from a connection.
	// Alternatively, we could have a thread that iterates through
	// the peer connections and checks each one for a message.
}
