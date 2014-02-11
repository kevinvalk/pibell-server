#pragma once

#ifdef CYGWIN
	#ifndef __MSABI_LONG
		#if defined(__MSC_VER) || defined(__MINGW32__)
			#define __MSABI_LONG(x)         x ## l
		#else
			#define __MSABI_LONG(x)         x
		#endif
	#endif
#endif

#define BOOST_FILESYSTEM_NO_DEPRECATED

// Interprocess has to be first in all files so just load them first in common
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/asio.hpp>

// C++ stuff
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <deque>
#include <thread>
#include <list>
#include <functional>

// Usings
using boost::asio::ip::tcp;

// Packet definitions
enum PacketType : unsigned int
{
	STATE = 1,
	REGISTER = 10,
	REGISTER_RESPONSE = 11,
	UNREGISTER = 11,
	CALL = 100,
	CLOSE = 9999
};

#pragma pack(push, 1)
struct PacketHeader
{
	PacketType type;
	unsigned int length;
};

struct PacketString
{
public:
	void set(std::string string)
	{
		set(string.c_str());
	}

	void set(const char* string)
	{
		len = std::strlen(string);

		if (len > maxStringLength)
		{
			// TODO: Error handling
		}

		std::memcpy(str, string, len + 1); // Include null
	}

	const char* get()
	{
		return str;
	}

	const unsigned short length()
	{
		return len;
	}
	enum { maxStringLength = 50 };

private:
	unsigned short len;
	char str[maxStringLength];
};

class Packet
{
public:
	enum { headerLength = sizeof(PacketHeader) };
	enum { maxBodyLength = 1024 };
	
	const char* getData() const
	{
		return data_;
	}

	char* getData()
	{
		return data_;
	}

	unsigned int getMaxLength() const
	{
		return headerLength + maxBodyLength;
	}

	const char* getBody() const
	{
		return data_ + headerLength;
	}

	char* getBody()
	{
		return data_ + headerLength;
	}

	PacketHeader *getHeader() const
	{
		return (PacketHeader*)data_;
	}

	PacketType getType() const
	{
		return *(PacketType*)(data_ + 0); // TODO: What is best practice, and C++11?
	}

	unsigned int getLength() const
	{
		return *(unsigned int*)(data_ + 4); // TODO: What is best practice, and C++11?
	}

	unsigned int getTotalLength() const
	{
		return getLength() + headerLength;
	}

private:
	char data_[headerLength + maxBodyLength];
};

struct PacketCall
{
private:
	PacketHeader header;

public:
	bool global;
	std::time_t time;

	PacketCall(bool global)
	{
		this->global = global;
		this->time = std::time(NULL);
		header.type = PacketType::CALL;
		header.length = sizeof(PacketCall) - Packet::headerLength;
	}
};

struct PacketClose
{
private:
	PacketHeader header;

public:
	PacketClose()
	{
		header.type = PacketType::CLOSE;
		header.length = sizeof(PacketClose) - Packet::headerLength;
	}
};

struct PacketState
{
private:
	PacketHeader header;

public:
	bool state;
	unsigned int errorCode = 0;

	PacketState(bool state)
	{
		this->state = state;
		header.type = PacketType::STATE;
		header.length = sizeof(PacketState) - Packet::headerLength;
	}
};

struct PacketRegister
{
private:
	PacketHeader header;

public:
	bool global;
	PacketString username;
	PacketString password;

	PacketRegister()
	{
		header.type = PacketType::REGISTER;
		header.length = sizeof(PacketRegister) - Packet::headerLength;
	}
};

struct PacketRegisterResponse
{
private:
	PacketHeader header;

public:
	unsigned int bellNo;
	bool hasGlobal;

	PacketRegisterResponse()
	{
		header.type = PacketType::REGISTER_RESPONSE;
		header.length = sizeof(PacketRegisterResponse) - Packet::headerLength;
	}
};
#pragma pack(pop)


// Type defs
typedef std::deque<Packet> PacketQueue;
#define PacketCast *(Packet*)&