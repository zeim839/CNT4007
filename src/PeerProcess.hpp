#ifndef CNT4007_PEERPROCESS_HPP
#define CNT4007_PEERPROCESS_HPP

#include "Protocol.hpp"
#include "FileSystem.hpp"
#include "PeerController.hpp"
#include "Log.hpp"

// PeerTableEntry tracks the state of some foreign peer.
struct PeerTableEntry
{
	unsigned int peerid        = 0;
	std::string  hostname      = "";
	unsigned int port          = 0;
	bool         isFinished    = false;
	bool         isChoking     = false;
	bool         isInterested  = true;
	bool         isChoked      = true;
	bool         isPreferred   = false;
	unsigned int bytesReceived = 0;

	PeerController* cntrl = NULL;
	std::unordered_map<unsigned int, bool> bitmap;
};

class PeerProcess
{
public:
	PeerProcess(unsigned int peerid);
	~PeerProcess();

private:

	// Peer State.
	unsigned int            peerid;
	unsigned int            port;
	ConfigCommon            cfgCommon;
	unsigned int            numPieces;
	std::vector<ConfigPeer> cfgPeers;
	bool                    selfHasFile;

	/*
	 * PeerInterface defines the API that is made available to
	 * peer controllers.
	 */
	PeerInterface ifc;

	// --- Protected by mutex ---
	std::mutex mu;
	unsigned char** file = NULL;
	bool selfFinished = false;
	unsigned int piecesDownloaded = 0;
	std::stack<std::pair<unsigned int, unsigned int>> interesting;
	std::unordered_map<unsigned int, PeerTableEntry> peerTable;
	// --------------------------

	// Concurrent routines.
	void server();
	void discover();
	void optimistic();
	void download();
	void selectPreferred();

	// Threads.
	std::thread thServer;
	std::thread thDiscover;
	std::thread thOptimistic;
	std::thread thSelectPreferred;
	std::thread thDownloader;

	// API.
	void setIsChoking(unsigned int peerid, bool isChoking);
	void setIsInterested(unsigned int peerid, bool isInterested);
	void setPeerHas(unsigned int peerid, unsigned int filepiece);
	void setBitfield(unsigned int peerid, std::string bitfield);
	void requestedPiece(unsigned int peerid, unsigned int filePiece);
	void receivedPiece(unsigned int peerid, std::string piece);

	// Utilities.

	// WARNING!!!: Must be called inside a mutex lock.
	std::string getBitfield();

	/*
	 * The process is finished when all peers (including this one)
	 * download the file.
	 */
	bool isFinished();

	// Clean up the process after finishing
	void terminate();

	// Broadcast that our peer has a piece.
	void broadcastHavePiece(unsigned int piece);

	// Try to exchange handshakes with the peer at socket.
	void xchgHandshakes(int socket, bool isOut);

	Log* log = NULL;
};

#endif // CNT4007_PEERPROCESS_HPP
