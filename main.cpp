#include "Common.h"
#include "Listener.h"

#define VERSION "1.0.0"

int main(int argc, char* argv[])
{
	std::cout << "Welcome to pibell-server version " << VERSION << "" << std::endl;
	try
	{
		// Create the server
		auto server = std::make_shared<Server>();

		// Set up the socket listener
		boost::asio::io_service ioService;
		tcp::endpoint endpoint(tcp::v4(), 4444);
		Listener listener(ioService, endpoint, server);
		boost::asio::signal_set signals(ioService, SIGINT, SIGTERM);

		signals.async_wait(std::bind(&boost::asio::io_service::stop, &ioService));

		// Run the server and the listener
		server->start();
		std::thread t([&ioService](){ ioService.run(); });

		// Wait for thread to terminate
		t.join();

		// Stop the server
		server->stop();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		exit(1);
	}

	// We are done !
	return 0;
}
