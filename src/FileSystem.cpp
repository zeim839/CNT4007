#include "FileSystem.hpp"

bool isNumber(const std::string& str)
{
	return !str.empty() && std::find_if(str.begin(),
                str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
}

std::pair<std::string, std::string> getCommonLine(std::ifstream& fin)
{
	std::string key = "";
	std::string val = "";
	std::string* current = &key;
	int c = -1;
	while(fin.good()) {
	        c = fin.get();
		if (c == ' ') {
			current = &val;
			continue;
		}

		if (c == '\n' || c == EOF)
			break;

		current->push_back(c);
	}

	return std::make_pair(key, val);
}

ConfigCommon FileSystem::loadCommonConfig(std::string path)
{
	std::ifstream fin(path.c_str(), std::ifstream::in);
	if (!fin.is_open()) {
		throw std::runtime_error("could not open common config at " +
                        path);
	}

	ConfigCommon config;

	// Parse NumberofPreferredNeighbors.
	std::pair<std::string, std::string> pair = getCommonLine(fin);
	if (pair.first != "NumberOfPreferredNeighbors") {
		throw std::runtime_error("Common.cfg: expected attribute "
                        "NumberofPreferredNeighbors but got " + pair.first);
	}

	if (!isNumber(pair.second)) {
		throw std::runtime_error("Common.cfg: expected value of "
                        "NumberOfPreferredNeighbors to be an integer");
	}

	config.numberOfPreferredNeighbors = std::stoi(pair.second);

	// Parse UnchokingInterval.
	pair = getCommonLine(fin);
	if (pair.first != "UnchokingInterval") {
		throw std::runtime_error("expected attribute UnchokingInterval "
                        "but got " + pair.first);
	}

	if (!isNumber(pair.second)) {
		throw std::runtime_error("Common.cfg: expected value of "
                        "UnchokingInterval to be an integer");
	}

	config.unchokingInterval = std::stoi(pair.second);

	// Parse OptimisticUnchokingInterval.
	pair = getCommonLine(fin);
	if (pair.first != "OptimisticUnchokingInterval") {
		throw std::runtime_error("expected attribute OptimisticUnchokingInterval "
                        "but got " + pair.first);
	}

	if (!isNumber(pair.second)) {
		throw std::runtime_error("Common.cfg: expected value of "
                        "OptimisticUnchokingInterval to be an integer");
	}

	config.optimisticUnchokingInterval = std::stoi(pair.second);

	// Parse file name.
	pair = getCommonLine(fin);
	if (pair.first != "FileName") {
		throw std::runtime_error("expected attribute FileName "
                        "but got " + pair.first);
	}

	config.fileName = pair.second;

	// Parse FileSize.
	pair = getCommonLine(fin);
	if (pair.first != "FileSize") {
		throw std::runtime_error("expected attribute FileSize "
                        "but got " + pair.first);
	}

	if (!isNumber(pair.second)) {
		throw std::runtime_error("Common.cfg: expected value of "
                        "FileSize to be an integer");
	}

	config.fileSize = std::stoi(pair.second);

	// Parse PieceSize.
	pair = getCommonLine(fin);
	if (pair.first != "PieceSize") {
		throw std::runtime_error("expected attribute PieceSize "
                        "but got " + pair.first);
	}

	if (!isNumber(pair.second)) {
		throw std::runtime_error("Common.cfg: expected value of "
                        "PieceSize to be an integer");
	}

	config.pieceSize = std::stoi(pair.second);
	return config;
}

ConfigPeer getPeerLine(std::ifstream& fin, unsigned int lineNum)
{
	ConfigPeer peer;

	std::string strPeerid  = "";
	std::string strHost    = "";
	std::string strPort    = "";
	std::string strHasFile = "";
	std::string* current   = &strPeerid;

	std::string* list[] = { &strPeerid, &strHost, &strPort, &strHasFile };
	unsigned int i = 1;

	while(fin.good()) {
		int c = fin.get();
		if (c == ' ') {
			current = list[i++];
			continue;
		}

		if (c == '\n' || c == EOF)
			break;

		current->push_back(c);
	}

	// Get peer id.
	if (!isNumber(strPeerid)) {
		throw std::runtime_error("PeerInfo.cfg:" + std::to_string(lineNum)
                        + ": expected peer ID to be an integer");
	}

	peer.peerid = std::stoi(strPeerid);

	// Get hostname.
	peer.hostname = strHost;

	// Get port.
	if (!isNumber(strPort)) {
		throw std::runtime_error("PeerInfo.cfg:" + std::to_string(lineNum)
                        + ": expected port to be an integer");
	}

	peer.port = std::stoi(strPort);

	// Get has file.
	if (!isNumber(strHasFile)) {
		throw std::runtime_error("PeerInfo.cfg:" + std::to_string(lineNum)
                        + ": expected 'has file' to be an integer");
	}

	unsigned int hasFile = std::stoi(strHasFile);
	if (hasFile != 1 && hasFile != 0) {
		throw std::runtime_error("PeerInfo.cfg:" + std::to_string(lineNum)
                        + ": expected 'has file' to be either 0 or 1");
	}

	peer.hasFile = hasFile;
	return peer;
}

std::vector<ConfigPeer> FileSystem::loadPeerConfig(std::string path)
{
	std::vector<ConfigPeer> peers;
	std::ifstream fin(path.c_str(), std::ifstream::in);
	if (!fin.is_open()) {
		throw std::runtime_error("could not open common config at " +
                        path);
	}

	unsigned int line = 1;
	while (fin.good()) {
		peers.push_back(getPeerLine(fin, line));
		++line;
	}

	return peers;
}

unsigned char** FileSystem::loadSharedFile(std::string path, unsigned int pieceSize)
{
	std::ifstream fin(path.c_str(), std::ifstream::in);
	if (!fin.is_open()) {
		throw std::runtime_error("could not open common config at " +
                        path);
	}

	std::vector<std::string> pieces;
	std::string currentPiece = "";
	int c = -1;
	while(fin.good()) {
		c = fin.get();
		if (c == EOF)
			break;
		currentPiece.push_back(c);
		if (currentPiece.size() == pieceSize) {
			pieces.push_back(currentPiece);
			currentPiece = "";
			continue;
		}
	}

	if (currentPiece != "") {
		while (pieceSize > currentPiece.size()) {
			currentPiece.push_back(0);
		}
		pieces.push_back(currentPiece);
	}

	std::cout << "Number of pieces loaded: " << pieces.size() << std::endl;

	unsigned int i = 0;
	unsigned char** out = new unsigned char*[pieces.size()];
	for (auto itr = pieces.begin(); itr != pieces.end(); ++itr) {
		unsigned char* p = new unsigned char[itr->size()];
		memcpy(p, itr->c_str(), itr->size());
		out[i] = p;
		++i;
	}

	return out;
}
