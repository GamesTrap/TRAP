////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2024 Laurent Gomila (laurent@sfml-dev.org)
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

//Modified by: Jan "GamesTrap" Schuerkamp

#include "TRAPPCH.h"
#include "IPv4Address.h"

#include "Network/HTTP/HTTP.h"
#include "Network/Sockets/SocketImpl.h"
#include "Utils/Utils.h"
#include "Utils/Memory.h"

TRAP::Optional<TRAP::Network::IPv4Address> TRAP::Network::IPv4Address::Resolve(const std::string& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	using namespace std::string_view_literals;

	if(address.empty())
	{
		//Not generating an error message here as resolution failure is a valid outcome.
		return TRAP::NullOpt;
	}

	if(address == "255.255.255.255"sv)
	{
		//The broadcast address needs to be handled explicitly,
		//because it is also the value returned by inet_addr on error.
		return Broadcast;
	}

	if(address == "0.0.0.0"sv)
		return Any;

	//Try to convert the address as a byte representation ("xxx.xxx.xxx.xxx")
	if(u32 ip = inet_addr(address.c_str()); ip != INADDR_NONE)
	{
		if constexpr (Utils::GetEndian() != Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(ip);

		return IPv4Address(ip);
	}

	//Not a valid address, try to convert it as a host name
	addrinfo hints{};
	hints.ai_family = AF_INET;

	addrinfo* result = nullptr;
	if(getaddrinfo(address.c_str(), nullptr, &hints, &result) == 0 && result != nullptr)
	{
		const sockaddr_in sin = std::bit_cast<sockaddr_in>(*(result->ai_addr));

		u32 ip = sin.sin_addr.s_addr;
		freeaddrinfo(result);

		if constexpr (Utils::GetEndian() != Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(ip);

		return IPv4Address(ip);
	}

	//Not generating an error message here as a resolution failure is a valid outcome.
	return TRAP::NullOpt;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Network::IPv4Address::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	in_addr address{};
	address.s_addr = m_address;

	if constexpr (Utils::GetEndian() == Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(address.s_addr);

	std::string str(16, '\0');
	inet_ntop(AF_INET, &address, str.data(), NumericCast<u32>(str.size()));
	std::erase(str, '\0');
	return str;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<TRAP::Network::IPv4Address> TRAP::Network::IPv4Address::GetLocalAddress()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//The method here is to connect a UDP socket to anyone (here to localhost),
	//and get the local socket address with the getsockname function.
	//UDP connection will not send anything to the network, so this function won't cause any overhead.

	//Create the socket
	const SocketHandle sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == INTERNAL::Network::SocketImpl::InvalidSocket())
	{
		TP_ERROR(TRAP::Log::NetworkIPv4AddressPrefix, "Failed to retrieve local address (invalid socket)");
		return TRAP::NullOpt;
	}

	//Connect the socket to localhost on any port
	u32 loopback = INADDR_LOOPBACK;
	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(loopback);

	const sockaddr_in address = INTERNAL::Network::SocketImpl::CreateAddress(loopback, 9);
	sockaddr convertedAddress = std::bit_cast<sockaddr>(address);
	if (connect(sock, &convertedAddress, sizeof(address)) == -1)
	{
		INTERNAL::Network::SocketImpl::Close(sock);

		TP_ERROR(TRAP::Log::NetworkIPv4AddressPrefix, "Failed to retrieve local address (socket connection failure)");
		return TRAP::NullOpt;
	}

	//Get the local address of the socket connection
	INTERNAL::Network::SocketImpl::AddressLength size = sizeof(address);
	if(getsockname(sock, &convertedAddress, &size) == -1)
	{
		INTERNAL::Network::SocketImpl::Close(sock);

		TP_ERROR(TRAP::Log::NetworkIPv4AddressPrefix, "Failed to retrieve local address (socket local address retrieval failure)");
		return TRAP::NullOpt;
	}

	//Close the socket
	INTERNAL::Network::SocketImpl::Close(sock);

	//Finally build the IP address
	u32 addr = std::bit_cast<sockaddr_in>(convertedAddress).sin_addr.s_addr;
	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(addr);

	return IPv4Address(addr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<TRAP::Network::IPv4Address> TRAP::Network::IPv4Address::GetPublicAddress(const Utils::TimeStep timeout)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//The trick here is more complicated, because the only way
	//to get our public IPv4 address is to get it from a distant computer.
	//Here we get the web page from http://trappedgames.de/ip-provider.php
	//and parse the result to extract our IP address
	//(not very hard: the web page contains only our IPv4 address).

	HTTP server{};
	server.SetHost("api.trappedgames.de", 80u, IPVersion::IPv4);
	HTTP::Request request("/ip-provider.php", HTTP::Request::Method::GET);
	HTTP::Response page = server.SendRequest(request, timeout);
	if(page.GetStatus() == HTTP::Response::Status::OK)
		return IPv4Address::Resolve(page.GetBody());

	//Retry with a different server
	server.SetHost("www.sfml-dev.org", 80u, IPVersion::IPv4);
	request.SetURI("/ip-provider.php");
	page = server.SendRequest(request, timeout);
	if (page.GetStatus() == HTTP::Response::Status::OK)
		return IPv4Address::Resolve(page.GetBody());

	//Something failed: return an invalid address
	TP_ERROR(TRAP::Log::NetworkIPv4AddressPrefix, "Failed to retrieve public address from external IP resolution server (HTTP response status ", page.GetStatus(), ")");
	return TRAP::NullOpt;
}

//-------------------------------------------------------------------------------------------------------------------//

std::istream& TRAP::Network::operator>>(std::istream& stream, TRAP::Optional<TRAP::Network::IPv4Address>& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	std::string str;
	stream >> str;
	address = TRAP::Network::IPv4Address::Resolve(str);

	return stream;
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& TRAP::Network::operator<<(std::ostream& stream, const TRAP::Network::IPv4Address& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	return stream << address.ToString();
}
