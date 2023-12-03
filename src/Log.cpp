#include "Log.hpp"

Log::Log(const std::string &path)
{
	this->fout.open(path, std::ofstream::out);
	if (!fout.is_open()) {
		std::cout << "warning: could not open log file at "
		        << path << " (log file will not be written)"
			<< std::endl;
	}

	this->isOpen = true;
}

Log::~Log()
{ this->close(); }

void Log::close()
{
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

	isOpen = false;
	fout.close();
	this->mu.unlock();
}

void Log::LogTCPOutgoing(unsigned int local, unsigned int remote)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::stringstream ss;
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

        ss << "[" << std::put_time(&tm, "%m-%d-%Y %H:%M:%S") <<
		"]: Peer " << local << " makes a connection to Peer " <<
		remote << "\n";

	this->fout << ss.str();
	std::cout << ss.str() << std::flush;
        this->mu.unlock();
}

void Log::LogTCPIncoming(unsigned int local, unsigned int remote)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::stringstream ss;
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

        ss << "[" << std::put_time(&tm, "%m-%d-%Y %H:%M:%S") <<
		"]: Peer " << local << " is connected to from " <<
		remote << "\n";

	this->fout << ss.str();
	std::cout << ss.str() << std::flush;
        this->mu.unlock();
}

void Log::LogChangePreferred
(unsigned int id, std::vector<unsigned int> peers)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	if (!peers.size())
		return;

	std::stringstream ss;
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

        ss << "[" << std::put_time(&tm, "%m-%d-%Y %H:%M:%S") <<
		"]: Peer " << id << " has preferred neighbors ";

	for (auto i = peers.begin(); i != peers.end(); ++i) {
	        ss << *i;
		if (i + 1 != peers.end()) {
		        ss << ", ";
		}
	}

        ss << "\n";
	this->fout << ss.str();
	std::cout << ss.str() << std::flush;
	this->mu.unlock();
}

void Log::LogChangeOptimistic(unsigned int local, unsigned int remote)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::stringstream ss;
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

        ss << "[" << std::put_time(&tm, "%m-%d-%Y %H:%M:%S") <<
		"]: Peer " << local << " has the optimistically "
		"unchoked neighbor " << remote << "\n";

	this->fout << ss.str();
	std::cout << ss.str() << std::flush;
        this->mu.unlock();
}

void Log::LogUnchoked(unsigned int local, unsigned int remote)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::stringstream ss;
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

	ss << "[" << std::put_time(&tm, "%m-%d-%Y %H:%M:%S") <<
		"]: Peer " << local << " is unchoked by " <<
		remote << "\n";

	this->fout << ss.str();
	std::cout << ss.str() << std::flush;
        this->mu.unlock();
}

void Log::LogChoked(unsigned int local, unsigned int remote)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::stringstream ss;
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

	ss << "[" << std::put_time(&tm, "%m-%d-%Y %H:%M:%S") <<
		"]: Peer " << local << " is choked by " <<
		remote << "\n";

	this->fout << ss.str();
	std::cout << ss.str() << std::flush;
        this->mu.unlock();
}

void Log::LogInterest
(unsigned int local, unsigned int remote, MsgType type)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::string typeStr;
	switch(type) {
	case MsgInterested:
		typeStr = "interested";
	case MsgUninterested:
		typeStr = "not interested";
	default:
		return;
	}

	std::stringstream ss;
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

	ss << "[" << std::put_time(&tm, "%m-%d-%Y %H:%M:%S") <<
		"]: Peer " << local << " received the '" << typeStr <<
		"' message from " << remote << "\n";

	this->fout << ss.str();
	std::cout << ss.str() << std::flush;
        this->mu.unlock();
}

void Log::LogHave
(unsigned int local, unsigned int remote, unsigned int piece)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::stringstream ss;
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

	ss << "[" << std::put_time(&tm, "%m-%d-%Y %H:%M:%S") <<
		"]: Peer " << local << " received the 'have' message"
		" from " << remote << " for the piece " <<
		piece << "\n";

	this->fout << ss.str();
	std::cout << ss.str() << std::flush;
        this->mu.unlock();
}

void Log::LogDownloaded
(unsigned int local, unsigned int remote,
 unsigned int piece, unsigned int total)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::stringstream ss;
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

	ss << "[" << std::put_time(&tm, "%m-%d-%Y %H:%M:%S") <<
		"]: Peer " << local << " has downloaded the piece " <<
		piece << " from " << remote << ". Now the number of "
		"pieces it has is " << total << "\n";

	this->fout << ss.str();
	std::cout << ss.str() << std::flush;
        this->mu.unlock();
}

void Log::LogCompleted(unsigned int local)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::stringstream ss;
	this->mu.lock();
	if (!this->isOpen) {
		this->mu.unlock();
		return;
	}

	ss << "[" << std::put_time(&tm, "%m-%d-%Y %H:%M:%S") <<
                "]: Peer " << local << " has downloaded the "
		"complete file\n";

	this->fout << ss.str();
	std::cout << ss.str() << std::flush;
        this->mu.unlock();
}
