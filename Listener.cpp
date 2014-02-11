#include "Listener.h"

Listener::Listener(boost::asio::io_service& io_service, const tcp::endpoint& endpoint, std::shared_ptr<Server> server)
	: acceptor_(io_service, endpoint), socket_(io_service)
{
	// Store the server
	server_ = server;

	// Wait for new clients
	do_accept();
}

Listener::~Listener()
{
}

void Listener::do_accept()
{
	acceptor_.async_accept(socket_, [this](boost::system::error_code errorCode)
	{
		// If everything ok, then create the new client
		if ( ! errorCode)
		{
			// Create a new client (shared pointer)
			auto p = std::make_shared<Client>(std::move(socket_), server_);
			p->start();

			// Add the client to the server
			server_->add(p);
		}

		// Wait for new clients
		do_accept();
	});
}
