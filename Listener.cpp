#include "Listener.h"

Listener::Listener(boost::asio::io_service& ioService, const tcp::endpoint& endpoint, std::shared_ptr<Server> server)
	: acceptor_(ioService, endpoint), socket_(ioService)
{
	// Store the server
	server_ = server;

	// Wait for new clients
	doAccept();
}

Listener::~Listener()
{
}

void Listener::doAccept()
{
	acceptor_.async_accept(socket_, [this](boost::system::error_code errorCode)
	{
		// If everything ok, then create the new client
		if ( ! errorCode)
		{
			// Create a client
			auto client = std::make_shared<Client>(std::move(socket_));

			// Link the open/close/packet handlers to the server
			client->setOpenHandler(std::bind(&Server::handleOpen, server_, std::placeholders::_1));
			client->setCloseHandler(std::bind(&Server::handleClose, server_, std::placeholders::_1));
			client->setPacketHandler(std::bind(&Server::handlePacket, server_, std::placeholders::_1, std::placeholders::_2));

			// Start the client
			client->start();
		}

		// Wait for new clients
		doAccept();
	});
}
