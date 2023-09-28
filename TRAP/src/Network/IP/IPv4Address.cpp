/*
////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2023 Laurent Gomila (laurent@sfml-dev.org)
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
#include "Utils/Utils.h"
#include "Utils/Memory.h"

constexpr TRAP::Network::IPv4Address TRAP::Network::IPv4Address::None{};
constexpr TRAP::Network::IPv4Address TRAP::Network::IPv4Address::Any(0, 0, 0, 0);
constexpr TRAP::Network::IPv4Address TRAP::Network::IPv4Address::LocalHost(127, 0, 0, 1);
constexpr TRAP::Network::IPv4Address TRAP::Network::IPv4Address::Broadcast(255, 255, 255, 255);

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv4Address::IPv4Address(const std::string_view address)
	: m_address(0), m_valid(false)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	Resolve(address);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Network::IPv4Address::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	in_addr address{};
	address.s_addr = m_address;

	std::string str(16, '\0');
	inet_ntop(AF_INET, &address, str.data(), NumericCast<uint32_t>(str.size()));
	std::erase(str, '\0');
	return str;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Network::IPv4Address TRAP::Network::IPv4Address::GetLocalAddress()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//The method here is to connect a UDP socket to anyone (here to localhost),
	//and get the local socket address with the getsockname function.
	//UDP connection will not send anything to the network, so this function won't cause any overhead.

	//Create the socket
	const SocketHandle sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == INTERNAL::Network::SocketImpl::InvalidSocket())
		return {};

	//Connect the socket to localhost on any port
	uint32_t loopback = INADDR_LOOPBACK;

	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(loopback);

	const sockaddr_in address = INTERNAL::Network::SocketImpl::CreateAddress(loopback, 9);
	sockaddr convertedAddress = std::bit_cast<sockaddr>(address);
	if (connect(sock, &convertedAddress, sizeof(address)) == -1)
	{
		INTERNAL::Network::SocketImpl::Close(sock);
		return {};
	}

	//Get the local address of the socket connection
	INTERNAL::Network::SocketImpl::AddressLength size = sizeof(address);
	if(getsockname(sock, &convertedAddress, &size) == -1)
	{
		INTERNAL::Network::SocketImpl::Close(sock);
		return {};
	}

	//Close the socket
	INTERNAL::Network::SocketImpl::Close(sock);

	//Finally build the IP address
	uint32_t addr = std::bit_cast<sockaddr_in>(convertedAddress).sin_addr.s_addr;

	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(addr);

	return IPv4Address(addr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Network::IPv4Address TRAP::Network::IPv4Address::GetPublicAddress(const Utils::TimeStep timeout)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

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
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::IPv4Address::Resolve(const std::string_view address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

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
		if((inet_pton(AF_INET, address.data(), &ip) != 0) && ip != INADDR_NONE)
		{
			m_address = ip;
			m_valid = true;
		}
		else
		{
			//Not a valid address, try to convert it as a host name
			addrinfo hints{};
			hints.ai_family = AF_INET;
			addrinfo* result = nullptr;
			if(getaddrinfo(address.data(), nullptr, &hints, &result) == 0)
			{
				if(result != nullptr)
				{
					ip = std::bit_cast<sockaddr_in>(*(result->ai_addr)).sin_addr.s_addr;
					freeaddrinfo(result);
					m_address = ip;
					m_valid = true;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::istream& TRAP::Network::operator>>(std::istream& stream, TRAP::Network::IPv4Address& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	std::string str;
	stream >> str;
	address = TRAP::Network::IPv4Address(str);

	return stream;
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& TRAP::Network::operator<<(std::ostream& stream, const TRAP::Network::IPv4Address& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	return stream << address.ToString();
}