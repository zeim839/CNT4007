#include "log.hpp"
#include <iostream>
#include <fstream>
using namespace std;

Log::Log(std::string path) {
	this->path = path;
	logMsg.open(path);
}

&ofstream Log::getStream(){
	return logMsg;
}

void Log::TCPconnection() {
	Log.getStream() << "[Time]: Peer [peer_ID 1] makes a connection to Peer [peer_ID 2].\n";
	logMsg.close();
}

void Log::changePrefNeighbors() {
	Log.getStream() << "[Time]: Peer [peer_ID] has the preferred neighbors [preferred neighbor ID list].\n";
	logMsg.close();
}

void Log::changeUnchokeNeighbors() {
	Log.getStream() << "[Time]: Peer [peer_ID] has the optimistically unchoked neighbor [optimistically unchoked neighbor ID].\n";
	logMsg.close();
}

void Log::unchoking() {
	Log.getStream() << "[Time]: Peer [peer_ID 1] is unchoked by [peer_ID 2].\n";
	logMsg.close();
}

void Log::choking() {
	Log.getStream() << "[Time]: Peer [peer_ID 1] is choked by [peer_ID 2].\n";
	logMsg.close();
}

void Log::haveMsg() {
	Log.getStream() << "[Time]: Peer [peer_ID 1] received the ‘have’ message from [peer_ID 2] for the piece [piece index].\n";
	logMsg.close();
}

void Log::interestMsg() {
	Log.getStream() << "[Time]: Peer [peer_ID 1] received the ‘interested’ message from [peer_ID 2].\n";
	logMsg.close();
}

void Log::notInterestMsg() {
	Log.getStream() << "[Time]: Peer [peer_ID 1] received the ‘not interested’ message from [peer_ID 2].\n";
	logMsg.close();
}

void Log::downloadPiece() {
	Log.getStream() << "[Time]: Peer [peer_ID 1] has downloaded the piece [piece index] from [peer_ID 2]. Now the number of pieces it has is [number of pieces].\n";
	logMsg.close();
}

void Log::downloadComplete() {
	Log.getStream() << "[Time]: Peer [peer_ID] has downloaded the complete file.\n";
	logMsg.close();
}



