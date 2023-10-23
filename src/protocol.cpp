#include "protocol.hpp"

Byte* MSG_HANDSHAKE::bytes()
{
	Byte* buffer = (Byte*)malloc(sizeof(Byte[32]));
	memcpy(buffer, &this->header, 18);
	memcpy(&buffer[18], &this->zeros, 10);
	memcpy(&buffer[28], &this->peerID, 4);
	return buffer;
}

MSG_HANDSHAKE MSG_HANDSHAKE::fromBytes(Byte* buffer)
{
	MSG_HANDSHAKE hs;
	memcpy(hs.header, buffer, 18);
	memcpy(hs.zeros, &buffer[18], 10);
	memcpy(&hs.peerID, &buffer[28], 4);
	return hs;
}

bool validateHandshake(MSG_HANDSHAKE hs)
{
	std::string header((char*)hs.header, 18);
	if (header != HANDSHAKE_HEADER)
		return false;

	// hs.zeros size == 10 bytes.
	for (int i = 0; i < 10; i++) {
		if (!hs.zeros[i])
			continue;
		return false;
	}

	return true;
}
