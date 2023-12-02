#ifndef CNT4007_LOG_HPP
#define CNT4007_LOG_HPP
#include "PeerController.hpp"
#include "PeerProcess.hpp"
#include <ctime>

class Log {
    public:
        Log(std::string path);
	    std::ofstream& getStream();

    private:     
        ofstream logMsg;
        string path;
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

        // current date and time on the current system
        time_t now = time(0);

        // convert now to string form
        char* date_time = ctime(&now);
};

#endif