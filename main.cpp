#include "src/Protocol.hpp"
#include "src/PeerProcess.hpp"

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cout << "error: missing peerid argument" << std::endl;
		std::cout << "usage: ./peerProcess [peerid]" << std::endl;
		return EXIT_FAILURE;
	}

	// Extract ID from cmd arguments.
	unsigned int id;
	try { id = std::stoi(argv[1]); } catch (...) {
		std::cout << "error: peerid argument must be an integer" << std::endl;
		std::cout << "usage: ./peerProcess [peerid]" << std::endl;
		return EXIT_FAILURE;
	}

	PeerProcess process(id);
}
