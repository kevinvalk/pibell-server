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
			// Create a client
			auto client = std::make_shared<Client>(std::move(socket_));

			// Link the open/close/packet handlers to the server
			client->setOpenHandler(std::bind(&Server::handleOpen, server_, client));
			client->setCloseHandler(std::bind(&Server::handleClose, server_, client));
			client->setPacketHandler(std::bind(&Server::handlePacket, server_, client, std::placeholders::_1));

			// Start the client
			client->start();
		}

		// Wait for new clients
		do_accept();
	});
}
