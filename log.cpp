#include "log.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

Log::Log(std::string path) {
	this->path = path;
	logMsg.open(path);
}

ofstream& Log::getStream() {
	return logMsg;
}

void Log::TCPconnection(remotePeerID, localPeerID) {
	this->getStream() << date_time << ": Peer " << localPeerID << " makes a connection to Peer " << remotePeerID << ".\n";
	logMsg.close();
}

void Log::changePrefNeighbors(localPeerID, neighborList) {
	this->getStream() << date_time << ": Peer " << localPeerID << " has the preferred neighbors " << neighborList << ".\n";
	logMsg.close();
}

void Log::changeUnchokeNeighbors(localPeerID, neighborID) {
	this->getStream() << date_time << ": Peer " << localPeerID << " has the optimistically unchoked neighbor " << neighborID << ".\n";
	logMsg.close();
}

void Log::unchoking(remotePeerID, localPeerID, msg) {
	if (msg.msgtype == Msg.Unchoke) {
		this->getStream() << date_time << ": Peer " << localPeerID << " is unchoked by " << remotePeerID << ".\n";
	}
	logMsg.close();
}

void Log::choking(remotePeerID, localPeerID, msg) {
	if (msg.msgtype = Msg.Choke) {
		this->getStream() << date_time << ": Peer " << localPeerID << " is choked by " << remotePeerID << ".\n";
	}
	logMsg.close();
}

void Log::haveMsg(remotePeerID, localPeerID, pieceIndex, msg) {
	if (msg.msgtype = Msg.Have) {
		this->getStream() << date_time << ": Peer " << localPeerID << " received the ‘have’ message from " << remotePeerID << " for the piece " << pieceIndex << ".\n";
	}
	logMsg.close();
}

void Log::interestMsg(remotePeerID, localPeerID, msg) {
	if (msg.msgtype = Msg.Interested) {
		this->getStream() << date_time << ": Peer " << localPeerID << " received the ‘interested’ message from " << remotePeerID << ".\n";
	}
	logMsg.close();
}

void Log::notInterestMsg(remotePeerId, localPeerID, msg) {
	if (msg.msgtype = Msg.UnInterested) {
		this->getStream() << date_time << ": Peer " << localPeerID << " received the ‘not interested’ message from " << remotePeerID << ".\n";
	}
	logMsg.close();
}

void Log::downloadPiece(remotePeerID, localPeerID, msg, pieceIndex, numPieces) {
	if (msg.msgType = Msg.Piece) {
		this->getStream() << date_time << ": Peer " << localPeerID << " has downloaded the piece " << pieceIndex " from " << remotePeerID << ". Now the number of pieces it has is " << numPieces << ".\n";
	}
	logMsg.close();
}

void Log::downloadComplete(peeriD) {
	this->getStream() << date_time << ": Peer " << peeriD << " has downloaded the complete file .\n";
	logMsg.close();
}



