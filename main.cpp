#include <system.hpp>
#include <network.hpp>
#include <process.hpp>

int main()
{
	// Start TCP server.
	CFG_COMMON config;
	std::vector<CFG_PEER> peers;
	PeerProcess p(3000, config, peers);
	std::cout << "server listening on port 3000" << std::endl;
	p.terminate();

	return 0;
}
