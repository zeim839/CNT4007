#include <system.hpp>
#include <network.hpp>
#include <process.hpp>
#include <chrono>

int main()
{
	// Start TCP server.
	CFG_COMMON config;
	std::vector<CFG_PEER> peers;
	PeerProcess p(1111, 3000, config, peers);
	std::cout << "server listening on port 3001" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(30));
	p.terminate();

	return 0;
}
