#include "config.hpp"

inline std::string __readLine
(std::ifstream& f, const std::string& path, const std::string& attr)
{
	std::string line;
	ASSERT(std::getline(f, line), "(%s) expected attribute %s",
		path.c_str(), attr.c_str());
	return line;
}

inline int __extractInt
(const std::string& line, const std::string& path, const std::string& attr)
{
	std::istringstream iss(line);
	std::string actualAttr;
	unsigned int value;
	ASSERT(iss >> actualAttr, "(%s) expected attribute %s but got %s",
	       path.c_str(), attr.c_str(), actualAttr.c_str());

	ASSERT(actualAttr == attr, "(%s) expected attribute %s but got %s",
	       path.c_str(), attr.c_str(), actualAttr.c_str());

	ASSERT(iss >> value, "(%s) expected integer value for attribute %s",
	       path.c_str(), attr.c_str());

	return value;

}

inline std::string __extractStr
(const std::string& line, const std::string& path, const std::string& attr)
{
	std::istringstream iss(line);
	std::string actualAttr, value;
	ASSERT(iss >> actualAttr, "(%s) expected attribute %s but got %s",
	       path.c_str(), attr.c_str(), actualAttr.c_str());

	ASSERT(actualAttr == attr, "(%s) expected attribute %s but got %s",
	       path.c_str(), attr.c_str(), actualAttr.c_str());

	ASSERT(iss >> value, "(%s) expected string value for attribute %s",
	       path.c_str(), attr.c_str());

	return value;
}

CFG_COMMON loadCommonConf(std::string path)
{
	std::ifstream f(path);
	ASSERT(f.is_open(), "(%s) expected common.cfg config file in "
	       "current working directory", path.c_str());

	CFG_COMMON config;
	std::string line;
	line = __readLine(f, path, "NumberOfPreferredNeighbors");
	config.numNeighbors = __extractInt(line, path,
                "NumberOfPreferredNeighbors");
	line.clear();

	line = __readLine(f, path, "UnchokingInterval");
	config.unchokingInt = __extractInt(line, path, "UnchokingInterval");
	line.clear();

	line = __readLine(f, path, "OptimisticUnchokingInterval");
	config.optUnchokingInt = __extractInt(line, path,
                "OptimisticUnchokingInterval");
	line.clear();

	line = __readLine(f, path, "FileName");
	config.fileName = __extractStr(line, path, "FileName");
	line.clear();

	line = __readLine(f, path, "FileSize");
	config.fileSize = __extractInt(line, path, "FileSize");
	line.clear();

	line = __readLine(f, path, "PieceSize");
	config.pieceSize = __extractInt(line, path, "PieceSize");
	line.clear();

	f.close();
	return config;
}

CFG_PEER __extractEntry
(std::ifstream& f, const std::string& line, unsigned int c, const std::string& path)
{
	CFG_PEER peer;
	std::istringstream iss(line);
	ASSERT(iss >> peer.peerId >> peer.hostname >> peer.port >> peer.hasFile,
	       "(%s:%d) could not read peer configuration", path.c_str(), c);

	return peer;
}

std::vector<CFG_PEER> loadPeerConf(std::string path)
{
	std::ifstream f(path);
	std::string line;
	std::vector<CFG_PEER> entries;
	unsigned int count = 1;
	while (std::getline(f, line)) {
		entries.push_back(__extractEntry(f, line, count, path));
		line.clear();
		count++;
	}

	return entries;
}
