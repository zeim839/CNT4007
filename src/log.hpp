#ifndef CNT4007_LOG_HPP
#define CNT4007_LOG_HPP

#include "system.hpp"

class Log
{
public:
	// Create a log instance that writes to the specified path.
	Log(std::string path);

	// Get the file stream.
	std::ofstream& getStream();

	// Close the file stream.
	void close();

	// TODO: convenience functions for common messages.
	void TCPconnection();
        void changePrefNeighbors();
        void changeUnchokeNeighbors();
        void unchoking();
        void choking();
        void haveMsg();
        void interestMsg();
        void notInterestMsg();
        void downloadPiece();
        void downloadComplete();

private:
	std::string path;
	std::ofstream logMsg;
};

#endif
