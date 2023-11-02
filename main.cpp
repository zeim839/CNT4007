#include <system.hpp>
#include <network.hpp>
#include <process.hpp>
#include <chrono>

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cout << "usage: ./peerProcess [port]" << std::endl;
		return EXIT_FAILURE;
	}

	// Extract ID from cmd arguments.
	int ID;
	try { ID = std::stoi(argv[1]); } catch (...) {
		std::cout << "usage: ./peerProcess [port]" << std::endl;
		return EXIT_FAILURE;
	}

	// Use TCP port 6008.
	int port = 6008;

	// Fetch common.cfg.
	CFG_COMMON config = loadCommonConf("./common.cfg");

	// Fetch Peerinfo.cfg.
	std::vector<CFG_PEER> peers = loadPeerConf("./Peerinfo.cfg");

	// Start peer process.
	PeerProcess p(ID, port, config, peers);

	std::cout << "server listening on port 3001" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(30));
	p.terminate();

	return EXIT_SUCCESS;
}
