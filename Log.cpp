#include "Log.hpp"

Log::Log(std::string path) {
	this->path = path;
	logMsg.open(path);
	if (!logMsg.is_open()) {
		throw std::runtime_error("could not open log file at " + path);
	}
}

std::ofstream& Log::getStream() {
	return logMsg;
}

void Log::getCurrentTime(){
	// current date and time on the current system
    now = time(0);

    // convert now to string form
    date_time = ctime(&now);
	_mutex.lock();
}

void Log::TCPconnection(unsigned int remotePeerID, unsigned int localPeerID) {
	getCurrentTime();
	this->getStream() << date_time << ": Peer " << localPeerID << " makes a connection to Peer " << remotePeerID << ".\n";
	std::cout << date_time << ": Peer " << localPeerID << " makes a connection to Peer " << remotePeerID << "." << std::endl;
	_mutex.unlock();


}

void Log::changePrefNeighbors(unsigned int localPeerID, std::vector <unsigned int> neighborList) {
	getCurrentTime();
	this->getStream() << date_time << ": Peer " << localPeerID << " has the preferred neighbors ";
	for (unsigned int i; i < neighborList.size(); ++i) {
		this->getStream() << neighborList[i] << ", ";
	}
	this->getStream() << ".\n";

	std::cout << date_time << ": Peer " << localPeerID << " has the preferred neighbors ";
	for (unsigned int i; i < neighborList.size(); ++i) {
		std::cout << neighborList[i] << ", ";
	}
	std::cout << "." << std::endl;
	_mutex.unlock();
}

void Log::changeUnchokeNeighbors(unsigned int localPeerID, unsigned int neighborID) {
	getCurrentTime();
	this->getStream() << date_time << ": Peer " << localPeerID << " has the optimistically unchoked neighbor " << neighborID << ".\n";
	std::cout << date_time << ": Peer " << localPeerID << " has the optimistically unchoked neighbor " << neighborID << "." << std::endl;
	_mutex.unlock();
}

void Log::unchoking(unsigned int remotePeerID, unsigned int localPeerID, struct GenericMsg msg) {
	getCurrentTime();
	msg = PeerController.msg;
	if (msg.msgtype == MsgUnchoke) {
		this->getStream() << date_time << ": Peer " << localPeerID << " is unchoked by " << remotePeerID << ".\n";
		std::cout << date_time << ": Peer " << localPeerID << " is unchoked by " << remotePeerID << "." << std::endl;
		_mutex.unlock();
	}
}

void Log::choking(unsigned int remotePeerID, unsigned int localPeerID, struct GenericMsg msg) {
	getCurrentTime();
	msg = PeerController.msg;
	if (msg.msgtype == MsgChoke) {
		this->getStream() << date_time << ": Peer " << localPeerID << " is choked by " << remotePeerID << ".\n";
		std::cout << date_time << ": Peer " << localPeerID << " is choked by " << remotePeerID << "." << std::endl;
		_mutex.unlock();
	}
}

void Log::haveMsg(unsigned int remotePeerID, unsigned int localPeerID, int pieceIndex, struct GenericMsg msg) {
	getCurrentTime();
	msg = PeerController.msg;
	if (msg.msgtype == MsgHave) {
		this->getStream() << date_time << ": Peer " << localPeerID << " received the ‘have’ message from " << remotePeerID << " for the piece " << pieceIndex << ".\n";
		std::cout << date_time << ": Peer " << localPeerID << " received the ‘have’ message from " << remotePeerID << " for the piece " << pieceIndex << "." << std::endl;
		_mutex.unlock();
	}
}

void Log::interestMsg(unsigned int remotePeerID, unsigned int localPeerID, struct GenericMsg msg) {
	getCurrentTime();
	msg = PeerController.msg;
	if (msg.msgtype == MsgInterested) {
		this->getStream() << date_time << ": Peer " << localPeerID << " received the ‘interested’ message from " << remotePeerID << ".\n";
		std::cout << date_time << ": Peer " << localPeerID << " received the ‘interested’ message from " << remotePeerID << "." << std::endl;
		_mutex.unlock();
	}
}

void Log::notInterestMsg(unsigned int remotePeerID, unsigned int localPeerID, struct GenericMsg msg) {
	getCurrentTime();
	msg = PeerController.msg;
	if (msg.msgtype == MsgUninterested) {
		this->getStream() << date_time << ": Peer " << localPeerID << " received the ‘not interested’ message from " << remotePeerID << ".\n";
		std::cout << date_time << ": Peer " << localPeerID << " received the ‘not interested’ message from " << remotePeerID << "." << std::endl;
		_mutex.unlock();
	}
}

void Log::downloadPiece(unsigned int remotePeerID, unsigned int localPeerID, struct GenericMsg msg, int pieceIndex, int numPieces) {
	getCurrentTime();
	msg = PeerController.msg;
	if (msg.msgtype == MsgPiece) {
		this->getStream() << date_time << ": Peer " << localPeerID << " has downloaded the piece " << pieceIndex << " from " << remotePeerID << ". Now the number of pieces it has is " << numPieces << ".\n";
		std::cout << date_time << ": Peer " << localPeerID << " has downloaded the piece " << pieceIndex << " from " << remotePeerID << ". Now the number of pieces it has is " << numPieces << "." << std::endl;
		_mutex.unlock();
	}
}

void Log::downloadComplete(unsigned int peeriD) {
	getCurrentTime();
	this->getStream() << date_time << ": Peer " << peeriD << " has downloaded the complete file .\n";
	std::cout << date_time << ": Peer " << peeriD << " has downloaded the complete file ." << std::endl;
	_mutex.unlock();
}

void Log::closeFile() {
	logMsg.close();
}



