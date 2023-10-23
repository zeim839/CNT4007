#ifndef CNT4007_PROTOCOL_HPP
#define CNT4007_PROTOCOL_HPP

#include "system.hpp"

const char HANDSHAKE_HEADER[] = "P2PFILESHARINGPROJ";

/*
 * The handshake message consists of three parts: handshake header,
 * zero bits, and peer ID. The handshake header is an 18-byte string
 * with value 'P2PFILESHARINGPROJ'.
 */
struct MSG_HANDSHAKE
{
	Byte header[18];
        Byte zeros[10];
	Quad peerID;

	/*
	 * Convert a handshake message into a 32-byte buffer. Must be
	 * deallocated after using.
	 */
	Byte* bytes();

	/*
	 * Convert a byte buffer to MSG_HANDSHAKE. The buffer must be at least
	 * 32 bytes in length. Buffer will not be free()d.
	 */
	static MSG_HANDSHAKE fromBytes(Byte* buffer);
};

/*
 * After handshaking, each peer can send a stream of actual messages.
 * An actual message consists of 4-byte message length field, 1-byte
 * message type field, and a message payload with variable size.
 */
struct MSG_DATA
{
	Quad  length;
	Byte  type;
	Byte* payload;
};

/*
 * The 1-byte message type field specifies the type of the message.
 * There are eight types of messages, with values in range [0,7].
 *
 *  - 'choke', 'unchoke', 'interested', 'uninterested' have no payload.
 *  - 'have' messages have a 4-byte index field payload.
 *  - 'bitfield' messages contain a bitfield of each segment of file
 *    data that the peer has or does not have.
 *  - ‘request’ messages have a payload which consists of a 4-byte
 *    piece index field.
 *  - ‘piece’ messages have a payload which consists of a 4-byte piece
 *    index field and the content of the piece.
 */
enum MSG_TYPE
{
	MTY_CHOKE, MTY_UNCHOKE,  MTY_INTERESTED, MTY_UNINTERESTED,
	MTY_HAVE,  MTY_BITFIELD, MTY_REQ,        MTY_PIECE,
};

/*
 * ValidateHandshake returns whether a handshake message follows protocol
 * specifications. It does not check whether the PeerID is a valid neighbor.
 */
bool validateHandshake(MSG_HANDSHAKE hs);

#endif // CNT4007_PROTOCOL_HPP
