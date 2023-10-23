#ifndef CNT4007_LOG_HPP
#define CNT4007_LOG_HPP

#include "system.hpp"


class Log {

public:
	Log(std::string path);
	std::ofstream& getStream();
	
private:
	ofstream logMsg;
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
};

#endif