#include "Server.h"

Server::Server()
{
}


Server::~Server()
{
}

void Server::start()
{
	// Start main thread
	thread_ = std::thread(&Server::run, this);
}

void Server::run()
{
	// TODO: Nicely cleaning
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		for (char k = 0x30; k < 0x40; ++k)
		{
			if (GetAsyncKeyState(k) & 0x8000)
			{
				unsigned int bellNo = k - 0x30;
				std::cout << "Pressed: " << bellNo << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(250));

				// 0 == global
				if (bellNo == 0)
				{
					for (auto client : globalBellClients_)
					{
						client->doCall(true);
					}
				}
				else
				{
					for (auto client : bellClients_[bellNo - 1])
					{
						client->doCall(false);
					}
				}
			}
		}

	}
}

short Server::login(std::shared_ptr<Client> client, std::string name, std::string password, bool global)
{
	if (name == "kevinvalk" && password == "asdasdasd")
	{
		std::cout << "Client successfully registered" << std::endl;
		bellClients_[0].push_back(client);
		if (global)
			globalBellClients_.push_back(client);
		return 0;
	}
	else
	{
		std::cout << "Invalid username/password (" << name << ":" << password << ")"  << std::endl;
	}
	return -1;
}

void Server::add(std::shared_ptr<Client> client)
{
	clients_.push_back(std::move(client));
}
