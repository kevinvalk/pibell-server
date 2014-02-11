#pragma once

#include "Common.h"
#include "Server.h"
#include "Client.h"

class Listener
{
public:
	Listener(boost::asio::io_service& ioService, const tcp::endpoint& endpoint, std::shared_ptr<Server> server);
	~Listener();

	void doAccept();

private:
	tcp::acceptor acceptor_;
	tcp::socket socket_;
	std::shared_ptr<Server> server_;
};
