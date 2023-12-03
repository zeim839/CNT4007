#ifndef CNT4007_LOG_HPP
#define CNT4007_LOG_HPP
#include "PeerController.hpp"
#include "PeerProcess.hpp"
#include "Protocol.hpp"
#include <ctime>

class Log {
    public:

        std::ofstream logMsg;
        std::string path;

        Log(std::string path);
	    std::ofstream& getStream();

        void getCurrentTime();
        void TCPconnection(unsigned int remotePeerID, unsigned int localPeerID);
        void changePrefNeighbors(unsigned int localPeerID, std::vector <unsigned int> neighborList);
        void changeUnchokeNeighbors(unsigned int localPeerID, unsigned int neighborID);
        void unchoking(unsigned int remotePeerID, unsigned int localPeerID, struct GenericMsg msg);
        void choking(unsigned int remotePeerID, unsigned int localPeerID, struct GenericMsg msg);
        void haveMsg(unsigned int remotePeerID, unsigned int localPeerID, int pieceIndex, struct GenericMsg msg);
        void interestMsg(unsigned int remotePeerID, unsigned int localPeerID, struct GenericMsg msg);
        void notInterestMsg(unsigned int remotePeerID, unsigned int localPeerID, struct GenericMsg msg);
        void downloadPiece(unsigned int remotePeerID, unsigned int localPeerID, struct GenericMsg msg, int pieceIndex, int numPieces);
        void downloadComplete(unsigned int peeriD);
        void closeFile();

    private:
        time_t now;
        char* date_time;
        std::mutex _mutex;

};

#endif