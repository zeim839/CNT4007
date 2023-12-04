#include "PeerController.hpp"

PeerController::PeerController(unsigned int peerid, int fd, PeerInterface ifc)
{
	this->peerid = peerid;
	this->socket = fd;
	this->ifc = ifc;
	this->closed = (fd < 0);
	if (this->closed)
		return;

	// Spawn listener thread.
	std::thread(&PeerController::listen, this).detach();
}

/*
 * TODO: closing this does not stop the detached listener thread. So
 * if the PeerController is deleted and listen() calls isClosed(),
 * then a segmentation fault is raised.
 */
void PeerController::close()
{
	this->closing.lock();
	if (this->isClosed()) {
		this->closing.unlock();
		return;
	}

	::close(this->socket);
	this->closed = true;
	this->closing.unlock();
}

void PeerController::listen()
{
	while (!this->isClosed()) {

		std::string buffer = "";
		if (!this->receive(buffer, 5))
			continue;

		const char* rawBuffer = buffer.c_str();

		GenericMsg msg;
		memcpy(&msg.length, rawBuffer, 4);
		memcpy(&msg.msgtype, rawBuffer + 4, 1);

		std::string payload;
		if (msg.length > 1 && !this->receive(payload, msg.length - 1))
			continue;

		unsigned char* payloadRaw = (unsigned char*)payload.c_str();

		// Handle messages.
		switch (msg.msgtype) {
		case MsgChoke:
			this->ifc.setIsChoking(this->peerid, true);
			break;
		case MsgUnchoke:
			this->ifc.setIsChoking(this->peerid, false);
			break;
		case MsgInterested:
			this->ifc.setIsInterested(this->peerid, true);
			break;
		case MsgUninterested:
			this->ifc.setIsInterested(this->peerid, false);
			break;
		case MsgHave: {
			if (payload.size() != 4)
				break;

			unsigned int filePiece;
			memcpy(&filePiece, payloadRaw, 4);
			this->ifc.setPeerHas(this->peerid, filePiece);
			break;
		}
		case MsgBitfield:
			if (payload.size() <= 0)
				break;

			std::cout << "Received bitfield from peer: " <<
				this->peerid << std::endl;

			this->ifc.setBitfield(this->peerid, payload);
			break;
		case MsgRequest: {
			if (payload.size() != 4)
				break;

			unsigned int filePiece;
			memcpy(&filePiece, payloadRaw, 4);
			this->ifc.requestedPiece(this->peerid, filePiece);
			break;
		}
		case MsgPiece:
			if (payload.size() <= 0)
				break;

			this->ifc.receivedPiece(this->peerid, payload);
			break;
		}
	}
}

bool PeerController::receive(std::string& out, unsigned int size)
{
	// Need to poll to ensure thread doesn't wait forever.
	signal(SIGPIPE, SIG_IGN);
	int remaining = size;
	while (!this->isClosed() && remaining > 0) {
		pollfd pfd;
		pfd.fd = this->socket;
		pfd.events = POLLIN;

		// Wait up to 250ms.
		if (poll(&pfd, 1, 250) <= 0)
			continue;

		char* buffer[remaining];
		int rcvd = recv(this->socket, &buffer, remaining, 0);
		if (rcvd <= 0 || errno == EPIPE) {
			this->close();
			return false;
		}

		std::string chunk((const char*)buffer, rcvd);
		out += chunk;
		remaining -= rcvd;
	}

	return true;
}

bool PeerController::sendMsg(GenericMsg msg, std::string payload)
{
	signal(SIGPIPE, SIG_IGN);

	if (this->isClosed())
		return false;

	if (payload.size() != (msg.length - 1))
		return false;

	std::string msgFull = std::string((const char*)&msg.length, 4) +
	        std::string((const char*)&msg.msgtype, 1);

	if (msg.length > 1) {
		msgFull += payload;
	}

	int sent = ::send(this->socket, msgFull.c_str(),
                sizeof(msg) + msg.length - 1, 0);

	if (sent < 0 || errno == EPIPE) {
		this->close();
		return false;
	}

	return true;
}

void PeerController::sendChoke()
{
	if (this->isClosed())
		return;

	GenericMsg msg;
	msg.length = 1;
	msg.msgtype = MsgChoke;
	sendMsg(msg, "");
}

void PeerController::sendUnchoke()
{
	if (this->isClosed())
		return;

	GenericMsg msg;
	msg.length = 1;
	msg.msgtype = MsgUnchoke;
	sendMsg(msg, "");
}

void PeerController::sendInterested()
{
	if (this->isClosed())
		return;

	GenericMsg msg;
	msg.length = 1;
	msg.msgtype = MsgInterested;
	sendMsg(msg, "");
}

void PeerController::sendUninterested()
{
	if (this->isClosed())
		return;

	GenericMsg msg;
	msg.length = 1;
	msg.msgtype = MsgUninterested;
	sendMsg(msg, "");
}

void PeerController::sendHave(unsigned int filePiece)
{
	if (this->isClosed())
		return;

	GenericMsg msg;
	msg.length = 5;
	msg.msgtype = MsgHave;

	unsigned char payloadRaw[4];
	memcpy(payloadRaw, &filePiece, 4);
	std::string payload((const char*)payloadRaw, 4);
	sendMsg(msg, payload);
}

void PeerController::sendBitfield(std::string bitfield)
{
	if (this->isClosed() || bitfield.size() <= 0)
		return;

	GenericMsg msg;
	msg.length = 1 + bitfield.size();
	msg.msgtype = MsgBitfield;
	sendMsg(msg, bitfield);
}

void PeerController::sendRequest(unsigned int filePiece)
{
	if (this->isClosed())
		return;

	GenericMsg msg;
	msg.length = 5;
	msg.msgtype = MsgRequest;

	unsigned char payloadRaw[4];
	memcpy(payloadRaw, &filePiece, 4);
	std::string payload((const char*)payloadRaw, 4);
	sendMsg(msg, payload);

	std::cout << "Requested piece " << filePiece << " from peer " <<
		this->peerid << std::endl;
}

void PeerController::sendPiece(std::string piece)
{
	if (this->isClosed())
		return;

	GenericMsg msg;
	msg.length = 1 + piece.size();
	msg.msgtype = MsgPiece;
	sendMsg(msg, piece);
}
