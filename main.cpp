#include <system.hpp>
#include <network.hpp>

int main()
{
	// Start TCP server.
	Server srv(3000);
	std::cout << "server listening on port 3000" << std::endl;
	while (true) {
		Connection* conn = srv.listen();
		std::cout << "incoming connection" << std::endl;

		// 1 KB message buffer.
		Byte buffer[1024];

		// Receive message.
		while (conn->receive(buffer, 1024)) {
			std::string str((char*)buffer);
			std::cout << "message from peer: " << str << std::endl;
		}
	}
	return 0;
}
