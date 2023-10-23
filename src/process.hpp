#ifndef CNT4007_PROCESS_HPP
#define CNT4007_PROCESS_HPP

#include "system.hpp"
#include "config.hpp"
#include "network.hpp"

/*
 * The peer process is the complete local server implementation of a
 * protocol peer.
 */
class PeerProcess: private Server
{
public:
	/*
	 * Start a peer process on the specified port, using the given
	 * configs. Starts accepting connections immediately.
	 * Throws if config or port are invalid.
	 */
	PeerProcess(unsigned int port, CFG_COMMON config,
		    std::vector<CFG_PEER> peers);

	/*
	 * config returns this peer's associated common.cfg
	 * configuration.
	 */
	CFG_COMMON config() { return this->_config; }

	// Close all peer connections and close the server socket.
	void terminate();

	// isClosed returns whether the PeerProcess is closed.
	bool isClosed() { return this->_isClosed; }

private:

	/*
	 * A listener waits for incoming connections and attempts to
	 * handshake with connecting peer. It is ran as a concurrent
	 * thread.
	 */
	void listener();

	/*
	 * The dialer runs when the PeerProcess is first instantiated.
	 * It attempts to establish a connection and handshake each
	 * peer in the list of peers. It is ran as a concurrent thread.
	 */
	void dialer(std::vector<CFG_PEER> peer);

	/*
	 * A receiver listens to a connection for messages, then
	 * validates, processes, and replies to them. It is assumed
	 * that the connection has passed handshaking. This is ran as
	 * a concurrent thread.
	 */
	void receiver(unsigned int id, Connection* conn);

	// The peer common.cfg configuration.
	CFG_COMMON _config;

	// The listener thread.
	std::thread thListener;

	// The dialer thread.
	std::thread thDialer;

	// The receiver thread.
	std::thread thReceiver;

	// Whether the peer is closed.
	bool _isClosed = false;

	// Maps peer IDs to connection pointers.
	std::unordered_map<unsigned int, Connection*> conns;

	// Maps peer IDs to CFG_PEER variables.
	std::unordered_map<unsigned int, CFG_PEER> peerConfigs;

	// TODO: bitfields.

	// Mutex.
	std::mutex mx;
};

#endif // CNT4007_PROCESS_HPP
