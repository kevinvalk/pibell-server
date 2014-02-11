#include "Client.h"
#include "Server.h"

Client::Client(tcp::socket socket, std::shared_ptr<Server> server)
	: socket_(std::move(socket))
{
	server_ = server;
}

Client::~Client()
{
	std::cout << "Client disconnected" << std::endl;
}

void Client::start()
{
	std::cout << "Client connected" << std::endl;
	doReceive();
}

void Client::send(Packet packet)
{
	bool isSending = ! sends_.empty();
	sends_.push_back(std::move(packet));
	if ( ! isSending)
	{
		doSend();
	}
}

void Client::doCall(bool global)
{
	std::cout << "Calling this client" << std::endl;
	PacketCall packetCall(global);
	send(PacketCast packetCall);
}

void Client::doReceive()
{
	auto self(shared_from_this());
	boost::asio::async_read(
		socket_,
		boost::asio::buffer(receive_.getData(), Packet::headerLength),
		[this, self](boost::system::error_code ec, std::size_t)
		{
			if ( ! ec)
			{
				handleReceive();
			}
			else
			{
				// TODO: Cleanup
				std::cout << "Receive error with code: " << ec.message() << std::endl;
			}
		}
	);
}

void Client::handleReceive()
{
	auto self(shared_from_this());
	boost::asio::async_read(
		socket_,
		boost::asio::buffer(receive_.getBody(), receive_.getLength()),
		[this, self](boost::system::error_code ec, std::size_t)
		{
			if ( ! ec)
			{
				bool success;
				std::cout << "Reveid packet with type:" << receive_.getType() << std::endl;
				switch (receive_.getType())
				{
					case PacketType::CALL:
						std::cout << "Receive call packet" << std::endl;
					break;
					case PacketType::REGISTER:
						success = handleRegister();
					break;
				}

				// Keep waiting for new stuff
				doReceive();
			}
			else
			{
				// TODO: Cleanup
				std::cout << "Handle receive error with code: " << ec << std::endl;
			}
		}
	);
}

bool Client::handleRegister()
{
	auto p = reinterpret_cast<PacketRegister*>(&receive_);
	std::cout << "Receive register for: " << p->username.get() << ", global: " << p->global << ", bellNo: " << std::endl;
	
	PacketState r(false);
	if (server_->login(shared_from_this(), std::string(p->username.get()), std::string(p->password.get()), p->global) >= 0)
	{
		r.state = true;
	}

	send(PacketCast r);

	return true;
}

void Client::doSend()
{
	auto self(shared_from_this());
	boost::asio::async_write(socket_,
		boost::asio::buffer(sends_.front().getData(), sends_.front().getTotalLength()),
		[this, self](boost::system::error_code ec, std::size_t)
		{
			if (!ec)
			{
				// Remove the sended message
				sends_.pop_front();

				// Keep sending if we have messages in queue
				if (! sends_.empty())
					return doSend();
			}
			else
			{
				// TODO: Cleanup
				std::cout << "Send error with code: " << ec << std::endl;
			}
		}
	);
}