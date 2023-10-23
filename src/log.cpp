#include log.hpp
#include <iostream>
#include <fstream>
using namespace std;

static void Log::TCPconnection() {
	logMsg.open(log_peer_[peerID].log);
	logMsg << "[Time]: Peer [peer_ID 1] makes a connection to Peer [peer_ID 2].\n";
	logMsg.close();
}

static void Log::changePrefNeighbors() {
	logMsg.open(log_peer_[peerID].log);
	logMsg << "[Time]: Peer [peer_ID] has the preferred neighbors [preferred neighbor ID list].\n";
	logMsg.close();
}

static void Log::changeUnchokeNeighbors() {
	logMsg.open(log_peer_[peerID].log);
	logMsg << "[Time]: Peer [peer_ID] has the optimistically unchoked neighbor [optimistically unchoked neighbor ID].\n";
	logMsg.close();
}

static void Log::unchoking() {
	logMsg.open(log_peer_[peerID].log);
	logMsg << "[Time]: Peer [peer_ID 1] is unchoked by [peer_ID 2].\n";
	logMsg.close();
}

static void Log::choking() {
	logMsg.open(log_peer_[peerID].log);
	logMsg << "[Time]: Peer [peer_ID 1] is choked by [peer_ID 2].\n";
	logMsg.close();
}

static void Log::haveMsg() {
	logMsg.open(log_peer_[peerID].log);
	logMsg << "[Time]: Peer [peer_ID 1] received the ‘have’ message from [peer_ID 2] for the piece [piece index].\n";
	logMsg.close();
}

static void Log::interestMsg() {
	logMsg.open(log_peer_[peerID].log);
	logMsg << "[Time]: Peer [peer_ID 1] received the ‘interested’ message from [peer_ID 2].\n";
	logMsg.close();
}

static void Log::notInterestMsg() {
	logMsg.open(log_peer_[peerID].log);
	logMsg << "[Time]: Peer [peer_ID 1] received the ‘not interested’ message from [peer_ID 2].\n";
	logMsg.close();
}

static void Log::downloadPiece() {
	logMsg.open(log_peer_[peerID].log);
	logMsg << "[Time]: Peer [peer_ID 1] has downloaded the piece [piece index] from [peer_ID 2]. Now the number of pieces it has is [number of pieces].\n";
	logMsg.close();
}

static void Log::downloadComplete() {
	logMsg.open(log_peer_[peerID].log);
	logMsg << "[Time]: Peer [peer_ID] has downloaded the complete file.\n";
	logMsg.close();
}



