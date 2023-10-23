#ifndef CNT4007_LOG_HPP
#define CNT4007_LOG_HPP

#include <fstream>


class Log {
    public:
	ofstream logMsg;
        static void TCPconnection();
        static void changePrefNeighbors();
        static void changeUnchokeNeighbors();
        static void unchoking();
        static void choking();
        static void haveMsg();
        static void interestMsg();
        static void notInterestMsg();
        static void downloadPiece();
        static void downloadComplete();
};

#endif