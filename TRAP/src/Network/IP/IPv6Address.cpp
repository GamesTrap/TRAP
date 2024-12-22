#include "TRAPPCH.h"
#include "IPv6Address.h"

#include "Network/HTTP/HTTP.h"
#include "Network/Sockets/SocketHandle.h"
#include "Network/Sockets/SocketImpl.h"
#include "Utils/String/String.h"

[[nodiscard]] TRAP::Optional<TRAP::Network::IPv6Address> TRAP::Network::IPv6Address::Resolve(const std::string& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	if(address.empty())
	{
		//Not generating an error message here as resolution failure is a valid outcome.
		return TRAP::NullOpt;
	}

	const std::string lowerAddress = Utils::String::ToLower(address);
	if (address == "::" || address == "0:0:0:0:0:0:0:0" ||
		address == "0000:0000:0000:0000:0000:0000:0000:0000")
	{
		return Any;
	}

	//Try to convert the address as a hex representation ("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")
	std::array<u8, 16> ip{};
	if(inet_pton(AF_INET6, lowerAddress.c_str(), ip.data()) != 0)
		return IPv6Address(ip);

	//Not a valid address, try to convert it as a host name
	addrinfo hints{};
	hints.ai_family = AF_INET6;

	addrinfo* result = nullptr;
	if(getaddrinfo(lowerAddress.c_str(), nullptr, &hints, &result) == 0 && result != nullptr)
	{
		const sockaddr_in6* const in6 = reinterpret_cast<sockaddr_in6*>(result->ai_addr);

		std::copy_n(in6->sin6_addr.s6_addr, ip.size(), ip.data());

		freeaddrinfo(result);

		return IPv6Address(ip);
	}

	//Not generating an error message here as a resolution failure is a valid outcome.
	return TRAP::NullOpt;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Network::IPv6Address::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	in6_addr address{};
#ifdef TRAP_PLATFORM_WINDOWS
	std::ranges::copy(m_address, address.u.Byte);
#else
	std::ranges::copy(m_address, address.s6_addr);
#endif

	//8 * 4 = 8 Blocks 4 Values each | 7 = 7 times ':'
	std::string str((8 * 4) + 7, 0);
	inet_ntop(AF_INET6, &address, str.data(), NumericCast<u32>(str.size()));
	std::erase(str, '\0');
	return str;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<TRAP::Network::IPv6Address> TRAP::Network::IPv6Address::GetLocalAddress()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//The method here is to connect a UDP socket to anyone (here to localhost),
	//and get the local socket address with the getsockname function.
	//UDP connection will not send anything to the network, so this function won't cause any overhead.

	//Create the socket
	const TRAP::Network::SocketHandle sock = socket(PF_INET6, SOCK_DGRAM, 0);
	if (sock == INTERNAL::Network::SocketImpl::InvalidSocket())
	{
		TP_ERROR(TRAP::Log::NetworkIPv6AddressPrefix, "Failed to retrieve local address (invalid socket)");
		return TRAP::NullOpt;
	}

	//Connect the socket to localhost on any port
	std::array<u8, 16> loopback{};
#ifdef TRAP_PLATFORM_WINDOWS
	std::copy_n(in6addr_loopback.u.Byte, loopback.size(), loopback.data());
#else
	std::copy_n(in6addr_loopback.s6_addr, loopback.size(), loopback.data());
#endif
	sockaddr_in6 address = INTERNAL::Network::SocketImpl::CreateAddress(loopback, 9);
	if(connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1)
	{
		INTERNAL::Network::SocketImpl::Close(sock);

		TP_ERROR(TRAP::Log::NetworkIPv6AddressPrefix, "Failed to retrieve local address (socket connection failure)");
		return TRAP::NullOpt;
	}

	//Get the local address of the socket connection
	INTERNAL::Network::SocketImpl::AddressLength size = sizeof(address);
	if(getsockname(sock, reinterpret_cast<sockaddr*>(&address), &size) == -1)
	{
		INTERNAL::Network::SocketImpl::Close(sock);

		TP_ERROR(TRAP::Log::NetworkIPv6AddressPrefix, "Failed to retrieve local address (socket local address retrieval failure)");
		return TRAP::NullOpt;
	}

	//Close the socket
	INTERNAL::Network::SocketImpl::Close(sock);

	//Finally build the IP address
	std::array<u8, 16> addr{};
#ifdef TRAP_PLATFORM_WINDOWS
	std::copy_n(address.sin6_addr.u.Byte, addr.size(), addr.data());
#else
	std::copy_n(address.sin6_addr.s6_addr, addr.size(), addr.data());
#endif
	return IPv6Address(addr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<TRAP::Network::IPv6Address> TRAP::Network::IPv6Address::GetPublicAddress(const Utils::TimeStep timeout)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//The trick here is more complicated, because the only way
	//to get our public IPv6 address is to get it from a distant computer.
	//Here we get the web page from http://trappedgames.de/ip-provider.php
	//and parse the result to extract our IP address
	//(not very hard: the web page contains only our IPv6 address).

	HTTP server{};
	server.SetHost("api.trappedgames.de", 80u, IPVersion::IPv6);
	HTTP::Request request("/ip-provider.php", TRAP::Network::HTTP::Request::Method::GET);
	HTTP::Response page = server.SendRequest(request, timeout);
	if(page.GetStatus() == HTTP::Response::Status::OK)
		return IPv6Address::Resolve(page.GetBody());

	//Retry with a different server
	server.SetHost("v6.ident.me", 80u, IPVersion::IPv6);
	request = HTTP::Request("", TRAP::Network::HTTP::Request::Method::GET);
	page = server.SendRequest(request, timeout);
	if(page.GetStatus() == HTTP::Response::Status::OK)
		return IPv6Address::Resolve(page.GetBody());

	//Something failed: return an invalid address
	TP_ERROR(TRAP::Log::NetworkIPv6AddressPrefix, "Failed to retrieve public address from external IP resolution server (HTTP response status", page.GetStatus(), ")");
	return TRAP::NullOpt;
}

//-------------------------------------------------------------------------------------------------------------------//

std::istream& TRAP::Network::operator>>(std::istream& stream, TRAP::Optional<TRAP::Network::IPv6Address>& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	std::string str;
	stream >> str;
	address = TRAP::Network::IPv6Address::Resolve(str);

	return stream;
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& TRAP::Network::operator<<(std::ostream& stream, const TRAP::Network::IPv6Address& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	return stream << address.ToString();
}
