#ifndef CNT4007_LOG_HPP
#define CNT4007_LOG_HPP

#include "Protocol.hpp"

class Log
{
public:
	Log(const std::string& path);
	~Log();

	// TCP Connection.
	void LogTCPOutgoing(unsigned int local, unsigned int remote);
	void LogTCPIncoming(unsigned int local, unsigned int remote);

	// Change of preferred neighbors.
	void LogChangePreferred(unsigned int id,
                std::vector<unsigned int> peers);

	// Change of optimistically unchoked neighbor.
	void LogChangeOptimistic(unsigned int local,
                unsigned int remote);

	// Unchoking/choking.
	void LogUnchoked(unsigned int local, unsigned int remote);
	void LogChoked(unsigned int local, unsigned int remote);

	// Received interested/uninterested message.
	void LogInterest(unsigned int local, unsigned int remote,
                MsgType type);

	// Have message.
	void LogHave(unsigned int local, unsigned int remote,
                unsigned int piece);

	// Downloaded a piece.
	void LogDownloaded(unsigned int local, unsigned int remote,
                unsigned int piece, unsigned int total);

	// Completed download.
	void LogCompleted(unsigned int local);

	// Close the file stream.
	void close();

private:
	std::mutex mu;
	bool isOpen = false;
	std::ofstream fout;
};

#endif // CNT4007_LOG_HPP
