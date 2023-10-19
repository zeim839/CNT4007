#ifndef CNT4007_CONFIG_HPP
#define CNT4007_CONFIG_HPP

#include "system.hpp"

// CFG_COMMON represents a common.cfg file in memory.
struct CFG_COMMON
{
	Quad numNeighbors;

	// Units are in seconds.
	Quad unchokingInt;
	Quad optUnchokingInt;

	// Name of file that all peers are interested in.
	std::string fileName;

	Quad fileSize;
	Quad pieceSize;
};

// CFG_PEER is a single entry in PeerInfo.cfg.
struct CFG_PEER
{
	Quad        peerId;
	std::string hostname;
	Quad        port;
	bool        hasFile;
};

// Load a common.cfg file from the specified path.
CFG_COMMON loadCommonConf(std::string path);

// Load a PeerInfo.cfg file from the specified path.
std::vector<CFG_PEER> loadPeerConf(std::string path);

#endif // CNT4007_CONFIG_HPP
