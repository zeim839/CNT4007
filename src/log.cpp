#include "log.hpp"

Log::Log(std::string path)
{
	this->path = path;
	this->logMsg.open(path, std::ofstream::out | std::ofstream::app);
	ASSERT(this->logMsg.is_open(), "could not open log file at %s",
	       path.c_str());
}

std::ofstream& Log::getStream()
{
	return this->logMsg;
}

void Log::close()
{
	this->logMsg.close();
}

void Log::TCPconnection()
{
	this->getStream() << "[Time]: Peer [peer_ID 1] makes a connection to Peer [peer_ID 2].\n";
	logMsg.close();
}

void Log::changePrefNeighbors()
{
	this->getStream() << "[Time]: Peer [peer_ID] has the preferred neighbors [preferred neighbor ID list].\n";
	logMsg.close();
}

void Log::changeUnchokeNeighbors()
{
	this->getStream() << "[Time]: Peer [peer_ID] has the optimistically unchoked neighbor [optimistically unchoked neighbor ID].\n";
	logMsg.close();
}

void Log::unchoking()
{
	this->getStream() << "[Time]: Peer [peer_ID 1] is unchoked by [peer_ID 2].\n";
	logMsg.close();
}

void Log::choking()
{
	this->getStream() << "[Time]: Peer [peer_ID 1] is choked by [peer_ID 2].\n";
	logMsg.close();
}

void Log::haveMsg()
{
	this->getStream() << "[Time]: Peer [peer_ID 1] received the ‘have’ message from [peer_ID 2] for the piece [piece index].\n";
	logMsg.close();
}

void Log::interestMsg()
{
	this->getStream() << "[Time]: Peer [peer_ID 1] received the ‘interested’ message from [peer_ID 2].\n";
	logMsg.close();
}

void Log::notInterestMsg()
{
	this->getStream() << "[Time]: Peer [peer_ID 1] received the ‘not interested’ message from [peer_ID 2].\n";
	logMsg.close();
}

void Log::downloadPiece()
{
	this->getStream() << "[Time]: Peer [peer_ID 1] has downloaded the piece [piece index] from [peer_ID 2]. Now the number of pieces it has is [number of pieces].\n";
	logMsg.close();
}

void Log::downloadComplete()
{
	this->getStream() << "[Time]: Peer [peer_ID] has downloaded the complete file.\n";
	logMsg.close();
}
