#include "process.hpp"

PeerProcess::PeerProcess
(unsigned int port, CFG_COMMON config, std::vector<CFG_PEER> peers) : Server(port)
{
	this->_config = config;

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

void PeerProcess::listener()
{
	while (!this->isClosed()) {
		Connection* conn = this->Server::listen();
		std::cout << "incoming connection" << std::endl;

		// TODO: Handshake.
	}
}

void PeerProcess::dialer(std::vector<CFG_PEER> peers)
{
	for (auto it = peers.begin(); it != peers.end(); ++it) {
		Connection* conn = NULL;
		try { conn = new Connection(*it); }
		catch (...) { continue; }

		// TODO: Handshake.
	}
}

void PeerProcess::receiver(unsigned int id, Connection* conn)
{
	// TODO.
}
