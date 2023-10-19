#include "protocol.hpp"

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
