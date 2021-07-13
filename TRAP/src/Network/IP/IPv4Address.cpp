/*
////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2019 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

Modified by: Jan "GamesTrap" Schuerkamp
*/

#include "TRAPPCH.h"
#include "IPv4Address.h"

#include "Network/HTTP/HTTP.h"
#include "Network/Sockets/SocketImpl.h"

const TRAP::Network::IPv4Address TRAP::Network::IPv4Address::None;
const TRAP::Network::IPv4Address TRAP::Network::IPv4Address::Any(0, 0, 0, 0);
const TRAP::Network::IPv4Address TRAP::Network::IPv4Address::LocalHost(127, 0, 0, 1);
const TRAP::Network::IPv4Address TRAP::Network::IPv4Address::Broadcast(255, 255, 255, 255);

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv4Address::IPv4Address()
	: m_address(0), m_valid(false)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv4Address::IPv4Address(const std::string_view address)
	: m_address(0), m_valid(false)
{
	Resolve(address);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv4Address::IPv4Address(const char* address)
	: m_address(0), m_valid(false)
{
	Resolve(address);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv4Address::IPv4Address(const uint8_t byte0, const uint8_t byte1, const uint8_t byte2,
                                        const uint8_t byte3)
	: m_address(htonl((byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3)), m_valid(true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv4Address::IPv4Address(const uint32_t address)
	: m_address(htonl(address)), m_valid(true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Network::IPv4Address::ToString() const
{
	in_addr address{};
	address.s_addr = m_address;

	std::string str(16, 0);
	inet_ntop(AF_INET, &address, str.data(), str.size());
	return str;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Network::IPv4Address::ToInteger() const
{
	return ntohl(m_address);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv4Address TRAP::Network::IPv4Address::GetLocalAddress()
{
	//The method here is to connect a UDP socket to anyone (here to localhost),
	//and get the local socket address with the getsockname function.
	//UDP connection will not send anything to the network, so this function won't cause any overhead.

	//Create the socket
	const SocketHandle sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == INTERNAL::Network::SocketImpl::InvalidSocket())
		return IPv4Address();

	//Connect the socket to localhost on any port
	sockaddr_in address = INTERNAL::Network::SocketImpl::CreateAddress(ntohl(INADDR_LOOPBACK), 9);
	if (connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1)
	{
		INTERNAL::Network::SocketImpl::Close(sock);
		return IPv4Address();
	}

	//Get the local address of the socket connection
	INTERNAL::Network::SocketImpl::AddressLength size = sizeof(address);
	if(getsockname(sock, reinterpret_cast<sockaddr*>(&address), &size) == -1)
	{
		INTERNAL::Network::SocketImpl::Close(sock);
		return IPv4Address();
	}

	//Close the socket
	INTERNAL::Network::SocketImpl::Close(sock);

	//Finally build the IP address
	return IPv4Address(ntohl(address.sin_addr.s_addr));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv4Address TRAP::Network::IPv4Address::GetPublicAddress(const Utils::TimeStep timeout)
{
	//The trick here is more complicated, because the only way
	//to get our public IPv4 address is to get it from a distant computer.
	//Here we get the web page from http://www.sfml-dev.org/ip-provider.php
	//and parse the result to extract our IP address
	//(not very hard: the web page contains only our IPv4 address).

	HTTP server("www.sfml-dev.org");
	const HTTP::Request request("/ip-provider.php", HTTP::Request::Method::GET);
	const HTTP::Response page = server.SendRequest(request, timeout);
	if (page.GetStatus() == HTTP::Response::Status::OK)
		return IPv4Address(page.GetBody());

	//Something failed: return an invalid address
	return IPv4Address();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::IPv4Address::Resolve(const std::string_view address)
{
	m_address = 0;
	m_valid = false;

	if(address == "255.255.255.255")
	{
		//The broadcast address needs to be handled explicitly,
		//because it is also the value returned by inet_addr on error
		m_address = INADDR_BROADCAST;
		m_valid = true;
	}
	else if(address == "0.0.0.0")
	{
		m_address = INADDR_ANY;
		m_valid = true;
	}
	else
	{
		//Try to convert the address as a byte representation ("xxx.xxx.xxx.xxx")
		uint32_t ip{};
		if(inet_pton(AF_INET, address.data(), &ip) && ip != INADDR_NONE)
		{
			m_address = ip;
			m_valid = true;
		}
		else
		{
			//Not a valid address, try to convert it as a host name
			addrinfo hints{};
			std::memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET;
			addrinfo* result = nullptr;
			if(getaddrinfo(address.data(), nullptr, &hints, &result) == 0)
			{
				if(result)
				{
					ip = reinterpret_cast<sockaddr_in*>(result->ai_addr)->sin_addr.s_addr;
					freeaddrinfo(result);
					m_address = ip;
					m_valid = true;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator==(const IPv4Address& left, const IPv4Address& right)
{
	return !(left < right) && !(right < left);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator!=(const IPv4Address& left, const IPv4Address& right)
{
	return !(left == right);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator<(const IPv4Address& left, const IPv4Address& right)
{
	return std::make_pair(left.m_valid, left.m_address) < std::make_pair(right.m_valid, right.m_address);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator>(const IPv4Address& left, const IPv4Address& right)
{
	return right < left;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator<=(const IPv4Address& left, const IPv4Address& right)
{
	return !(right < left);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator>=(const IPv4Address& left, const IPv4Address& right)
{
	return !(left < right);
}

//-------------------------------------------------------------------------------------------------------------------//

std::istream& TRAP::Network::operator>>(std::istream& stream, IPv4Address& address)
{
	std::string str;
	stream >> str;
	address = IPv4Address(str);

	return stream;
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& TRAP::Network::operator<<(std::ostream& stream, const IPv4Address& address)
{
	return stream << address.ToString();
}