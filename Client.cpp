#include "Client.h"

Client::Client(tcp::socket socket)
	: socket_(std::move(socket))
{
	bellNo = -1;
	bellGlobal = false;
	isRegistered = false;
}

Client::~Client()
{
	std::cout << "Client destructed!!" << std::endl;
}

void Client::start()
{
	handleOpen(shared_from_this());
	doReceive();
}

void Client::stop()
{
	std::cout << "Stopping the client" << std::endl;

	// Call the server to release me
	handleClose(shared_from_this());

	// Close the socket
	socket_.close();
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

void Client::call(bool global)
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
				stop();
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
				handlePacket(self, &receive_);

				// Keep waiting for new stuff
				doReceive();
			}
			else
			{
				stop();
			}
		}
	);
}

void Client::doSend()
{
	auto self(shared_from_this());
	boost::asio::async_write(
		socket_,
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
				std::cerr << "Error: " << ec.message() << std::endl;
				stop();
			}
		}
	);
}

// Properties
void Client::setOpenHandler(std::function<bool(std::shared_ptr<Client>)> handler)
{
	handleOpen = handler;
}

void Client::setCloseHandler(std::function<bool(std::shared_ptr<Client>)> handler)
{
	handleClose = handler;
}

void Client::setPacketHandler(std::function<bool(std::shared_ptr<Client>, Packet*)> handler)
{
	handlePacket = handler;
}
