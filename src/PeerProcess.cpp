#include "PeerProcess.hpp"

PeerProcess::PeerProcess(unsigned int peerid)
{
	this->peerid = peerid;

	// Load Common.cfg.
	this->cfgCommon = FileSystem::loadCommonConfig("Common.cfg");

	// Load peers.
	this->cfgPeers = FileSystem::loadPeerConfig("PeerInfo.cfg");

	// Initialize log file.
	std::string logFilePath = "log_peer_" +
		std::to_string(this->peerid) + ".log";

	this->log = new Log(logFilePath);

	// Make peer directory.
	std::string peerHomeDir = "peer_" + std::to_string(this->peerid);
	mkdir(peerHomeDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	// Initialize peer table.
	bool foundSelf = false;
	for (auto itr = this->cfgPeers.begin(); itr != this->cfgPeers.end(); ++itr) {
		if (itr->peerid == this->peerid) {
			foundSelf = true;
			this->selfHasFile = itr->hasFile;
			this->port = itr->port;
			continue;
		}

		if (peerTable.contains(itr->peerid)) {
			throw std::runtime_error("Duplicate peer entry: " +
                            std::to_string(itr->peerid));
		}

		PeerTableEntry entry;
		entry.peerid = itr->peerid;
		entry.hostname = itr->hostname;
		entry.port = itr->port;
		this->peerTable[itr->peerid] = entry;
	}

	if (!foundSelf) {
		throw std::runtime_error("could not find self (id: " +
                        std::to_string(this->peerid) + ") in PeerInfo.cfg");
	}

	// Calculate upper boundary of number of pieces.
	this->numPieces = std::ceil(((float)this->cfgCommon.fileSize /
                (float)this->cfgCommon.pieceSize));

	if (selfHasFile) {
		this->selfFinished = true;
		std::string fileName = "peer_" +
			std::to_string(this->peerid) +
			"/" + this->cfgCommon.fileName;

		this->file = FileSystem::loadSharedFile(fileName,
                        this->cfgCommon.pieceSize);

		this->piecesDownloaded = this->numPieces;
		this->log->LogCompleted(this->peerid);
	}

	// Initialize file.
	this->file = (!this->file) ? new unsigned char*[this->numPieces]
		: this->file;

	// If we don't have the file, set each piece pointer to NULL.
	for (unsigned int i = 0; (i < this->numPieces && !selfHasFile); ++i)
		this->file[i] = NULL;

	// Initialize interface.
	this->ifc.setIsChoking = std::bind(&PeerProcess::setIsChoking,
                this, std::placeholders::_1, std::placeholders::_2);

	this->ifc.setIsInterested = std::bind(&PeerProcess::setIsInterested,
                this, std::placeholders::_1, std::placeholders::_2);

	this->ifc.setPeerHas = std::bind(&PeerProcess::setPeerHas,
                this, std::placeholders::_1, std::placeholders::_2);

	this->ifc.setBitfield = std::bind(&PeerProcess::setBitfield,
                this, std::placeholders::_1, std::placeholders::_2);

	this->ifc.requestedPiece = std::bind(&PeerProcess::requestedPiece,
                this, std::placeholders::_1, std::placeholders::_2);

	this->ifc.receivedPiece = std::bind(&PeerProcess::receivedPiece,
                this, std::placeholders::_1, std::placeholders::_2);

        // Start server thread.
	this->thServer = std::thread(&PeerProcess::server, this);

	// Start discovery thread.
	this->thDiscover = std::thread(&PeerProcess::discover, this);

	// Start optimistic peer thread.
	this->thOptimistic = std::thread(&PeerProcess::optimistic, this);

	// Start unchoke peers thread.
	this->thSelectPreferred = std::thread(&PeerProcess::selectPreferred, this);

	// Start downloader thread.
	this->thDownloader = std::thread(&PeerProcess::download, this);

	// Wait for child threads.
	this->thServer.join();
	this->thDiscover.join();
	this->thOptimistic.join();
	this->thDownloader.join();
	this->thSelectPreferred.join();

	if (!this->selfHasFile)
		this->log->LogCompleted(this->peerid);

	// Finished. Start cleanup.
	std::cout << "TERMINATING..." << std::endl;
}

PeerProcess::~PeerProcess()
{ this->terminate(); }

void PeerProcess::server()
{
	sockaddr_in servAddr;
	bzero((char*)&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(this->port);

	// Open TCP connection.
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
		throw std::runtime_error("could not establish server socket");

	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) < 0) {
		::close(serverSocket);
		throw std::runtime_error("could not establish server socket");
	}

	// Bind socket to local address.
	int bindStatus = bind(serverSocket, (struct sockaddr*)&servAddr,
                sizeof(servAddr));

	if (bindStatus < 0) {
		throw std::runtime_error("could not bind socket to given peer"
                        " port and hostname");
	}

	// Maintain up to 30 incoming connection requests at a time.
	listen(serverSocket, 30);

	while (!this->isFinished()) {
		sockaddr_in inAddr;
		socklen_t inAddrSize = sizeof(inAddr);
		int inSocket = accept(serverSocket, (sockaddr*)&inAddr,
                        &inAddrSize);

		if (inSocket < 0)
			continue;

		this->xchgHandshakes(inSocket, false);
	}
}

void PeerProcess::discover()
{
	while(!this->isFinished()) {
		for (auto i = cfgPeers.begin(); i != cfgPeers.end(); ++i) {
			// cfgPeers, so expect local peer.
			if (i->peerid == this->peerid)
				continue;

			if (this->peerTable[i->peerid].isFinished)
				continue;

			if (this->peerTable[i->peerid].cntrl) {

				// Peer disconnected.
				if (this->peerTable[i->peerid].cntrl->isClosed()) {
					this->mu.lock();
					delete this->peerTable[i->peerid].cntrl;
					this->peerTable[i->peerid].isChoking     = false;
					this->peerTable[i->peerid].isChoked      = true;
					this->peerTable[i->peerid].isPreferred   = false;
					this->peerTable[i->peerid].bytesReceived = 0;
					this->peerTable[i->peerid].cntrl         = NULL;
					this->mu.unlock();
				}

				continue;
			}

			// Resolve peer hostname.
			sockaddr_in addr;
			hostent* info = gethostbyname(i->hostname.c_str());
			if (!info)
				continue;

			// Create dialing socket.
			int sout = socket(AF_INET, SOCK_STREAM, 0);
			if (sout < 0)
				continue;

			if (fcntl(sout, F_SETFL, O_NONBLOCK) < 0) {
				::close(sout);
				continue;
			}

			// Set up peer address structure.
			bzero((char*)&addr, sizeof(addr));
			addr.sin_family = AF_INET;
			bcopy((char*)info->h_addr, (char*)&addr.sin_addr.s_addr,
			      info->h_length);

			addr.sin_port = htons(i->port);

			// Connect to server.
			int conn = connect(sout, (struct sockaddr*)&addr,
                                sizeof(addr));

			if (conn < 0 && errno == EINPROGRESS) {
				timeval tv;
				tv.tv_sec = 2; // Wait up to 2 seconds.
				tv.tv_usec = 0;

				fd_set fdset;
				FD_ZERO(&fdset);
				FD_SET(sout, &fdset);

				conn = select(sout + 1, NULL, &fdset, NULL, &tv);

                                // An error occurred or select timed out.
				if (conn <= 0) {
					::close(sout);
					continue;
				}

			} else if (conn < 0) {
				::close(sout);
				continue;
			}

			// Check for socket-level errors.
			int opt = 1;
			socklen_t len = sizeof(opt);
			if (getsockopt (sout, SOL_SOCKET, SO_ERROR, &opt, &len) < 0) {
				::close(sout);
				continue;
			}

			// there was an error
			if (opt) {
				::close(sout);
				continue;
			}

			// Set socket back to blocking mode
			fcntl(sout, F_SETFL, 0);

			// Exchange handshakes.
			this->xchgHandshakes(sout, true);
		}
	}
}

void PeerProcess::optimistic()
{
	int optimisticPeerid = -1;
	while(!this->isFinished()) {
		if (optimisticPeerid >= 0) {
			std::this_thread::sleep_for(std::chrono::seconds
                                (this->cfgCommon.optimisticUnchokingInterval));
		}

		this->mu.lock();
		std::vector<unsigned int> candidates;
		for (auto it = peerTable.begin(); it != peerTable.end(); ++it) {
			if (it->second.isChoked && it->second.isInterested &&
			    !it->second.isPreferred && it->second.cntrl) {
				candidates.push_back(it->second.peerid);
			}
		}

		// If no candidates, reuse the old optimistic peer.
		if (candidates.empty()) {
			this->mu.unlock();
			continue;
		}

		// Randomly pick optimistic peer.
		::srand((unsigned)time(NULL));
		size_t index = rand() % candidates.size();

		// Choke old peer, if its not preferred.
		if (this->peerTable.contains(optimisticPeerid) &&
		    !this->peerTable[optimisticPeerid].isPreferred) {

			this->peerTable[optimisticPeerid].isChoked = true;
			if (this->peerTable[optimisticPeerid].cntrl) {
				this->peerTable[optimisticPeerid].cntrl->sendChoke();
			}
		}

		optimisticPeerid = candidates[index];

		// Unchokes new optimistic peer.
		this->peerTable[optimisticPeerid].isChoked = false;
		this->peerTable[optimisticPeerid].cntrl->sendUnchoke();
		this->mu.unlock();

		this->log->LogChangeOptimistic(this->peerid, optimisticPeerid);
	}
}

void PeerProcess::selectPreferred()
{
	std::unordered_map<unsigned int, bool> oldSelected;
	while (!this->isFinished()) {
		std::this_thread::sleep_for(std::chrono::seconds
                        (this->cfgCommon.unchokingInterval));

		this->mu.lock();

		// Calculate rates.
		std::vector<float> rates;
		std::unordered_map<float, std::vector<PeerTableEntry>> peersByRate;
		for (auto i = this->peerTable.begin(); i != this->peerTable.end(); ++i) {
			if (!i->second.cntrl || !i->second.isInterested || i->second.isFinished)
				continue;

			float rate = i->second.bytesReceived /
				this->cfgCommon.unchokingInterval;

			peersByRate[rate].push_back(i->second);
			rates.push_back(rate);
		}

		// Sort rates in descending order.
		std::sort(rates.begin(), rates.end(), std::greater<float>());

		unsigned int count = 0;
		std::unordered_map<unsigned int, bool> selected;
		std::vector<unsigned int> toLog;
		for (auto i = rates.begin(); (i != rates.end() &&
                        count < this->cfgCommon.numberOfPreferredNeighbors); ++i) {

			std::vector<PeerTableEntry> peers = peersByRate[*i];
			for (auto j = peers.begin(); (j != peers.end() &&
                                count < this->cfgCommon.numberOfPreferredNeighbors); ++j) {

				selected[j->peerid] = true;
				oldSelected[j->peerid] = false;

				if (this->peerTable[j->peerid].isChoked) {
					this->peerTable[j->peerid].cntrl->sendUnchoke();
				}

				toLog.push_back(j->peerid);
				this->peerTable[j->peerid].isChoked = false;
				++count;
			}
		}

		this->log->LogChangePreferred(this->peerid, toLog);

		// Choke old peers.
		for (auto i = oldSelected.begin(); i != oldSelected.end(); ++i) {
			if (i->second && this->peerTable[i->first].cntrl) {
				this->peerTable[i->first].isChoked = true;
				this->peerTable[i->first].cntrl->sendChoke();
			}
		}

		oldSelected = selected;
		this->mu.unlock();
	}
}


void PeerProcess::xchgHandshakes(int socket, bool isOut)
{
	signal(SIGPIPE, SIG_IGN);

	// Proactively send handshake.
	const char zeros[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::string hsMsg = "P2PFILESHARINGPROJ" + std::string(zeros, 10) +
		std::to_string(this->peerid);

	int sentHandshake = send(socket, hsMsg.c_str(), hsMsg.size(), 0);
	if (sentHandshake < 0 || errno == EPIPE) {
		::close(socket);
		return;
	}

	/*
	 * Set deadline (1500ms) for incoming handshake, otherwise any
	 * other waiting peers will be indefinitely starved.
	 */
	pollfd pfd;
	pfd.fd = socket;
	pfd.events = POLLIN;

	if (poll(&pfd, 1, 1500) <= 0) {
		::close(socket);
		return;
	}

	// Await incoming handshake.
	// TODO: This might be problematic if the message arrives in
	// multiple parts.
	char buffer[32];
	int rcvd = recv(socket, &buffer, 32, 0);
	if (rcvd < 32 || errno == EPIPE) {
		::close(socket);
		return;
	}

	// Marshall message.
	HandshakeMsg inHsMsg;
	inHsMsg.header = std::string((const char*)buffer, 18);
	memcpy(inHsMsg.zeros, buffer + 18, 10);

	// Last 4 bytes are the integer representation (i.e. string).
	std::string peeridStr((const char*)buffer + 28, 4);

	// Convert to integer.
	try { inHsMsg.peerid = std::stoi(peeridStr); } catch (...) {
		::close(socket);
		return;
	}

	if (inHsMsg.header != HANDSHAKE_HEADER) {
		::close(socket);
		return;
	}

	std::cout << "Handshake magic from peer " << inHsMsg.peerid << ": "
		  << inHsMsg.header << std::endl;

	bool allZeros = true;
	for (int i = 0; i < 10; i++) {
		if (inHsMsg.zeros[i] != 0) {
			allZeros = false;
			break;
		}
	}

	if (!allZeros) {
		::close(socket);
		return;
	}

	// PeerID should be known.
	if (!this->peerTable.contains(inHsMsg.peerid)) {
		::close(socket);
		return;
	}

	this->mu.lock();

	// Connection has already been established.
	if (this->peerTable[inHsMsg.peerid].cntrl) {
		this->mu.unlock();
		::close(socket);
		return;
	}

	if (isOut) {
		this->log->LogTCPOutgoing(this->peerid,
                        inHsMsg.peerid);
	} else {
		this->log->LogTCPIncoming(this->peerid,
                        inHsMsg.peerid);
	}

	this->peerTable[inHsMsg.peerid].cntrl = new PeerController
		(inHsMsg.peerid, socket, this->ifc);

	// Send bitfield.
	this->peerTable[inHsMsg.peerid].cntrl->sendBitfield
		(this->getBitfield());

	std::cout << "Peer " << this->peerid << " sent bitfield to remote peer "
		  << inHsMsg.peerid << std::endl;

	this->mu.unlock();
}

void PeerProcess::download()
{
	std::unordered_map<unsigned int, unsigned int> piecePeer;
	std::unordered_map<unsigned int, unsigned int> peerPiece;
	while (!this->isFinished()) {
		if (this->interesting.empty()) {
			continue;
		}

		this->mu.lock();

		// Randomize.
		std::vector<std::pair<unsigned int, unsigned int>> shuffledInt;
		while (this->interesting.size()) {
			shuffledInt.push_back(this->interesting.top());
			this->interesting.pop();
		}

		std::shuffle(shuffledInt.begin(), shuffledInt.end(),
		        std::default_random_engine(0));

		for (auto i = shuffledInt.begin(); i != shuffledInt.end(); ++i) {
			this->interesting.push(*i);
		}

		// Try to identify as many interesting pieces as
		// possible.
		std::stack<std::pair<unsigned int, unsigned int>> putBack;
		while(this->interesting.size()) {
			std::pair<unsigned int, unsigned int> intPiece = this->interesting.top();
			this->interesting.pop();

			// Already have piece or we don't know the peer.
			if (this->file[intPiece.second] ||
			    !this->peerTable.contains(intPiece.first))
				continue;

			putBack.push(intPiece);

			// Cannot download.
			if (!this->peerTable[intPiece.first].cntrl ||
			    this->peerTable[intPiece.first].isChoking) {
				continue;
			}

			// Piece is already being downloaded.
			if (piecePeer.contains(intPiece.second)) {
				continue;
			}

			// Already requested peer.
			if (peerPiece.contains(intPiece.first)) {
				continue;
			}

			// Request piece.
			piecePeer[intPiece.second] = intPiece.first;
			peerPiece[intPiece.first] = intPiece.second;
			this->peerTable[intPiece.first].cntrl->sendRequest(intPiece.second);
		}

		// Put back interesting pieces.
		while(putBack.size()) {
			this->interesting.push(putBack.top());
			putBack.pop();
		}

		this->mu.unlock();

		/*
		 * We assume a download rate of 50KBPS from each peer
		 * and attempt to calculate the average download time
		 * for each piece, with a minimum of 300ms and a maximum
		 * of (unchokingInterval + 5) seconds.
		 */
		int msAvg = (this->cfgCommon.pieceSize / 20000) * 1000;
		int wait = msAvg;

		if (msAvg < 500) {
			wait = 500;
		} else if (wait > (this->cfgCommon.unchokingInterval + 5) * 1000) {
			wait = (this->cfgCommon.unchokingInterval + 5) * 1000;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(wait));

		// Release locks and try to download more pieces.
		peerPiece.clear();
		piecePeer.clear();
	}
}

void PeerProcess::setIsChoking(unsigned int peerid, bool isChoking)
{
	this->mu.lock();
	if (this->peerTable.contains(peerid))
		this->peerTable[peerid].isChoking = isChoking;

	this->mu.unlock();

	if (isChoking) {
		this->log->LogChoked(this->peerid, peerid);
	} else {
		this->log->LogUnchoked(this->peerid, peerid);
	}
}

void PeerProcess::setIsInterested(unsigned int peerid, bool isInterested)
{
	this->mu.lock();
	if (this->peerTable.contains(peerid))
		this->peerTable[peerid].isInterested = isInterested;

	this->mu.unlock();

	MsgType type = (isInterested) ? MsgInterested :
		MsgUninterested;

	this->log->LogInterest(this->peerid, peerid, type);
}

void PeerProcess::setPeerHas(unsigned int peerid, unsigned int filepiece)
{
	this->mu.lock();
	if (!this->peerTable.contains(peerid)) {
		this->mu.unlock();
		return;
	}

	this->peerTable[peerid].bitmap[filepiece] = true;
	if (!this->file[filepiece]) {
		this->interesting.push(std::make_pair(peerid, filepiece));
		this->peerTable[peerid].cntrl->sendInterested();
	}

	this->mu.unlock();
	this->log->LogHave(this->peerid, peerid,  filepiece);

	std::cout << "Received 'have' message from peer " <<
		peerid << ". Remote peer bitfield index " <<
		filepiece << " is now set to true." << std::endl;
}

void PeerProcess::setBitfield(unsigned int peerid, std::string bitfield)
{
	this->mu.lock();
	if (!this->peerTable.contains(peerid)) {
		this->mu.unlock();
		return;
	}

	/*
	 * In the event of a reconnection, no assumptions can be made
	 * on whether the peer is finished.
	 */
	this->peerTable[peerid].isFinished = false;

	unsigned int i = 0;
	bool isInteresting = false;
	for (auto itr = bitfield.begin(); (itr != bitfield.end() && i < this->numPieces); ++itr) {
		for (int k = 0; k < 8; ++k) {
			unsigned char bit = (unsigned char)(*itr) & (1 << (7-k));
			this->peerTable[peerid].bitmap[i] = bit;
			if (this->peerTable[peerid].bitmap[i] && this->file[i] == NULL) {
				this->interesting.push(std::make_pair(peerid, i));
				isInteresting = true;
			}

			if (++i == this->numPieces)
				break;
		}
	}

	if (isInteresting) {
		this->peerTable[peerid].cntrl->sendInterested();
		this->mu.unlock();
		return;
	}

	this->peerTable[peerid].cntrl->sendUninterested();
	this->mu.unlock();
}

void PeerProcess::requestedPiece(unsigned int peerid, unsigned int filePiece)
{
	this->mu.lock();
	if (!this->peerTable.contains(peerid) || filePiece > this->numPieces) {
		this->mu.unlock();
		return;
	}

	if (this->peerTable[peerid].isChoked || this->file[filePiece] == NULL) {
		this->mu.unlock();
		return;
	}

	unsigned char piece[4 + this->cfgCommon.pieceSize];
	memcpy(piece, &filePiece, 4);
	memcpy(piece + 4, this->file[filePiece], this->cfgCommon.pieceSize);
	std::string pieceStr((const char*)piece, 4 + this->cfgCommon.pieceSize);

	this->peerTable[peerid].cntrl->sendPiece(pieceStr);
	this->mu.unlock();

	std::cout << "Sent piece " << filePiece << " to peer " << peerid
		  << std::endl;
}

void PeerProcess::receivedPiece(unsigned int peerid, std::string piece)
{
	this->mu.lock();
	if (!this->peerTable.contains(peerid) ||
	    piece.size() < 4 + this->cfgCommon.pieceSize) {
		this->mu.unlock();
		return;
	}

	unsigned char* pieceRaw = (unsigned char*)piece.c_str();
	unsigned int pieceNum;
	memcpy(&pieceNum, pieceRaw, 4);
	if (pieceNum >= this->numPieces || this->file[pieceNum]) {
		this->mu.unlock();
		return;
	}

	unsigned char* dynPiece = new unsigned char[this->cfgCommon.pieceSize];
	memcpy(dynPiece, pieceRaw + 4, this->cfgCommon.pieceSize);
	this->file[pieceNum] = dynPiece;

	this->peerTable[peerid].bytesReceived +=
		this->cfgCommon.pieceSize;

	this->piecesDownloaded += 1;

	this->mu.unlock();

	// Broadcast that we have received a piece.
	this->broadcastHavePiece(pieceNum);

	this->log->LogDownloaded(this->peerid, peerid,
                pieceNum, this->piecesDownloaded);
}

// WARNING: must be called inside a mutex lock.
std::string PeerProcess::getBitfield()
{
	if (!this->file) {
		throw std::runtime_error("attempted to read uninitialized "
                        "bitfield");
	}

	unsigned int bytes = std::ceil(((float)this->numPieces / 8.0f));
	std::string bitfield = "";
	unsigned char byte = 0;
	unsigned int j = 0;

	for (unsigned int i = 0; i < this->numPieces; ++i) {
		if (j == 8) {
			bitfield += byte;
			byte = 0;
			j = 0;
		}

		byte = (this->file[i]) ? (byte | (1 << (7 - j))) : byte;
		++j;
	}

	if (bitfield.size() < bytes)
		bitfield.push_back(byte);

	return bitfield;
}

bool PeerProcess::isFinished()
{
	this->mu.lock();

	// Check if our peer has all file pieces.
	bool haveAllPieces = true;
	for (unsigned int i = 0; (i < this->numPieces && !this->selfFinished); ++i) {
		if (this->file[i] == NULL) {
			haveAllPieces = false;
			break;
		}
	}

	if (!haveAllPieces) {
		this->mu.unlock();
		return false;
	}

	if (haveAllPieces && !this->selfFinished) {
		this->selfFinished = true;
		for (auto itr = this->peerTable.begin(); itr != this->peerTable.end(); ++itr) {
			if (itr->second.cntrl) itr->second.cntrl->sendUninterested();
		}
	}

	// Check if all peers have the file.
	bool hasUnfinished = false;
	for (auto itr = this->peerTable.begin(); itr != this->peerTable.end(); ++itr) {
		if (itr->second.isFinished || !itr->second.isInterested)
			continue;

		bool peerHasUnfinished = false;
		for (unsigned int i = 0; i < this->numPieces; ++i) {
			if (!itr->second.bitmap[i]) {
				hasUnfinished = true;
				peerHasUnfinished = true;
				break;
			}
		}

		itr->second.isFinished = (!peerHasUnfinished);
	}

	if (hasUnfinished) {
		this->mu.unlock();
		return false;
	}

	this->mu.unlock();
	return true;
}

void PeerProcess::terminate()
{
	this->mu.lock();

	// Close peer connections.
	for (auto i = this->peerTable.begin(); i != this->peerTable.end(); ++i) {
		if (i->second.cntrl)
			i->second.cntrl->close();
	}

	// Write the file to the peer's directory.
	if (!this->selfHasFile && this->selfFinished) {
		std::string fileName = "peer_" +
			std::to_string(this->peerid) +
			"/" + this->cfgCommon.fileName;

		FileSystem::writeSharedFile(fileName, this->file,
                        this->cfgCommon.pieceSize, this->numPieces);
	}

	// Delete file from memory.
	for (unsigned int i = 0; i < this->numPieces; ++i) {
		if (this->file[i] != NULL)
			delete[] this->file[i];
	}
	delete[] this->file;

	// Delete peer connections (after some delay).
	for (auto i = this->peerTable.begin(); i != this->peerTable.end(); ++i) {
		delete i->second.cntrl;
	}

	delete this->log;
	this->mu.unlock();
}

void PeerProcess::broadcastHavePiece(unsigned int piece)
{
	this->mu.lock();
	for (auto itr = this->peerTable.begin(); itr != this->peerTable.end(); ++itr) {
		if (itr->second.cntrl)
			itr->second.cntrl->sendHave(piece);
	}

	this->mu.unlock();
}
