#pragma once

#include "Client.h"

class Server
{
public:
	Server();
	~Server();

	void start();
	void add(std::shared_ptr<Client> client);
	short login(std::shared_ptr<Client> client, std::string name, std::string password, bool global);

private:
	void run();

	std::thread thread_;
	std::list<std::shared_ptr<Client>> clients_;

	std::list<std::shared_ptr<Client>> globalBellClients_;
	std::map<unsigned short, std::list<std::shared_ptr<Client>>> bellClients_;
};

