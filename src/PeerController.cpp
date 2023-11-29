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
	this->listener = std::thread(&PeerController::listen, this);
}

void PeerController::close()
{
	this->closing.lock();
	if (this->isClosed()) {
		this->closing.unlock();
		return;
	}

	::close(this->socket);
	this->closed = true;

	// Wait for thread to finish.
	if (this->listener.joinable())
		this->listener.join();

	this->closing.unlock();
}

void PeerController::listen()
{
	while (!this->isClosed()) {

		std::string buffer = "";
		if (!this->receive(buffer, 5))
			continue;

		unsigned char* rawBuffer = (unsigned char*)buffer.c_str();
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
	int remaining = size;
	while (!this->isClosed() && remaining > 0) {
		pollfd pfd;
		pfd.fd = this->socket;
		pfd.events = POLLIN;

		// Wait up to 250ms.
		if (poll(&pfd, 1, 250) <= 0)
			continue;

		char* buffer[size];
		int rcvd = recv(this->socket, &buffer, sizeof(buffer), 0);
		if (rcvd <= 0) {
			this->close();
			return false;
		}

		std::string chunk((const char*)buffer, size);
		out += chunk;
		remaining -= rcvd;
	}

	return false;
}

bool PeerController::sendMsg(GenericMsg msg, std::string payload)
{
	if (this->isClosed())
		return false;

	if (payload.size() != (msg.length - 1))
		return false;

	char* msgFull = new char[sizeof(msg) + msg.length - 1];
	memcpy(msgFull, &msg.length, 4);
	memcpy(msgFull + 4, &msg.msgtype, 1);
	if (msg.length > 1) {
		const char* rawPayload = payload.c_str();
		memcpy(msgFull + 5, rawPayload, msg.length - 1);
	}

	int sent = ::send(this->socket, msgFull, sizeof(msg) + msg.length - 1, 0);
	delete[] msgFull;

	if (sent < 0) {
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
