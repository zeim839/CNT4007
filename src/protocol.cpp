#include "protocol.hpp"

bool ValidateHandshake(MSG_HANDSHAKE hs)
{
	std::string header(hs.header, 18);
	if (header != HANDSHAKE_HEADER)
		return false;

	for (int i = 0; i < hs.zeros; i++) {
		if (!hs.zeros[i])
			continue;
		return false;
	}

	return true;
}
