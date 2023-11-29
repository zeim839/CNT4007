#ifndef CNT4007_FILESYSTEM_HPP
#define CNT4007_FILESYSTEM_HPP

#include "Protocol.hpp"

// FileSystem is a static class for handling file I/O.
class FileSystem
{
public:
	/*
	 * loadCommonConfig attempts to open a Common.cfg file at the
	 * specified path. It ensures that the file follows the
	 * common.cfg specification (or throws an error otherwise) and
	 * returns the corresponding ConfigCommon struct.
	 */
	static ConfigCommon loadCommonConfig(std::string path);

	/*
	 * loadPeerConfig attempts to read a PeerInfo.cfg file at the
	 * specified path. It ensures that each entry is valid and
	 * adds it to a vector with all other peer configurations.
	 * Throws if the file cannot be opened or is invalid.
	 */
	static std::vector<ConfigPeer> loadPeerConfig(std::string path);

	/*
	 * loadSharedFile loads the file at path and writes pieceSize
	 * bytes into each index of a bitfield. Throws if the file
	 * cannot be opened.
	 */
	static unsigned char** loadSharedFile(std::string path, unsigned int pieceSize);
};

#endif // CNT4007_FILESYSTEM_HPP
