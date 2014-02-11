#pragma once

#include "Common.h"
#include "Server.h"
#include "Client.h"

class Listener
{
public:
	Listener(boost::asio::io_service& io_service, const tcp::endpoint& endpoint, std::shared_ptr<Server> server);
	~Listener();

	void do_accept();

private:
	tcp::acceptor acceptor_;
	tcp::socket socket_;
	std::shared_ptr<Server> server_;
};
