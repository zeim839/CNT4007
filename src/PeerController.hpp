#ifndef CNT4007_PEERCONTROLLER_HPP
#define CNT4007_PEERCONTROLLER_HPP

#include "Protocol.hpp"

class PeerController;

/*
 * Callback functions for passing information from the PeerController
 * to the parent PeerProcess. See the prototypes in PeerProcess.hpp.
 */
struct PeerInterface
{
	std::function<void(unsigned int, bool)>         setIsChoking;
	std::function<void(unsigned int, bool)>         setIsInterested;
	std::function<void(unsigned int, unsigned int)> setPeerHas;
	std::function<void(unsigned int, std::string)>  setBitfield;
	std::function<void(unsigned int, unsigned int)> requestedPiece;
	std::function<void(unsigned int, std::string)>  receivedPiece;
};

/*
 * PeerController asynchronously handles messages from the remote
 * peer. When a message arrives, it calls one of PeerInterface to
 * inform the parent PeerProcess.
 */
class PeerController
{
public:
	PeerController(unsigned int peerid, int fd, PeerInterface ifc);

	// Close the connection. Cannot be re-opened.
	void close();

	// Whether the connection is closed or not.
	bool isClosed()
	{ return this->closed; }

	// Send the choke message to the remote peer.
	void sendChoke();

	// Send the unchoke message to the remote peer.
	void sendUnchoke();

	// Send the interested message to the remote peer.
	void sendInterested();

	// Send the not-interested message to the remote peer.
	void sendUninterested();

	/*
	 * Send the have message to the remote peer. Caller must
	 * ensure filePiece actually exists.
	 */
	void sendHave(unsigned int filePiece);

	/*
	 * Sends the bitfield string to the remote peer. Caller must
	 * ensure bitfield is the correct size.
	 */
	void sendBitfield(std::string bitfield);

	/*
	 * Sends the request message to the remote peer. Caller must
	 * ensure filePiece actually exists.
	 */
	void sendRequest(unsigned int filePiece);

        /*
         * Sends the piece string to the remote peer. Caller must
	 * ensure piece is the correct size.
	 */
	void sendPiece(std::string piece);

private:

	int socket = -1;
	unsigned int peerid = 0;
	bool closed = true;

	/*
	 * ifc contains callback functions for passing messages back
	 * to the parent PeerProcess.
	 */
	PeerInterface ifc;

	// The listen() thread.
	std::thread listener;

	/*
	 * Will wait for a message of the specified size to arrive, as
	 * long as the connection remains open. If the message is
	 * received, it is written to out and true is returned.
	 */
	bool receive(std::string& out, unsigned int size);

	// Listen for incoming messages.
	void listen();

	// Protects close() from accidentally being called concurrently.
	std::mutex closing;

	/*
	 * Sends msg to the remote peer, along with the payload, if
	 * applicable. Returns true/false on success/failure.
	 */
	bool sendMsg(GenericMsg msg, std::string payload);
};

#endif // CNT4007_PEERCONTROLLER_HPP
