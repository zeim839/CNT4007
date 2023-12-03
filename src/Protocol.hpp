#ifndef CNT4007_PROTOCOL_HPP
#define CNT4007_PROTOCOL_HPP

#include <string>
#include <vector>
#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <functional>
#include <fstream>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <random>
#include <chrono>
#include <algorithm>
#include <stack>
#include <sys/stat.h>

#define HANDSHAKE_HEADER "P2PFILESHARINGPROJ"

// The protocol message types.
#pragma pack(1)
enum MsgType
{
  MsgChoke, MsgUnchoke,  MsgInterested, MsgUninterested,
  MsgHave,  MsgBitfield, MsgRequest,    MsgPiece,
};

// The handshake message structure.
#pragma pack(1)
struct HandshakeMsg
{
	std::string  header;    // Must equal HANDSHAKE_HEADER.
        char         zeros[10]; // 10 bytes of zeros.
	unsigned int peerid;    // 4-byte peer ID.
};
#pragma pack(0)

// The generic message structure.
#pragma pack(1)
struct GenericMsg
{
	unsigned int length;  // 4-byte length prefix.
	char         msgtype; // 1-byte msg type, must be one of MsgType.
};
#pragma pack(0)

// The structure of configuration file common.cfg.
#pragma pack(1)
struct ConfigCommon
{
	unsigned int numberOfPreferredNeighbors;
	unsigned int unchokingInterval;
	unsigned int optimisticUnchokingInterval;
	std::string  fileName;
	unsigned int fileSize;
	unsigned int pieceSize;
};
#pragma pack(0)

// ConfigPeer is the structure of a single entry in PeerInfo.cfg.
#pragma pack(1)
struct ConfigPeer
{
	unsigned int peerid;
	std::string  hostname;
	unsigned int port;
	bool         hasFile;
};
#pragma pack(0)

#endif // CNT4007_PROTOCOL_HPP
