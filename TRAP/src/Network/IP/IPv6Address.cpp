#include "TRAPPCH.h"
#include "IPv6Address.h"

#include "Network/HTTP/HTTP.h"
#include "Network/Sockets/SocketHandle.h"
#include "Network/Sockets/SocketImpl.h"
#include "Utils/String/String.h"

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::IPv6Address TRAP::Network::IPv6Address::None{};
constexpr TRAP::Network::IPv6Address TRAP::Network::IPv6Address::Any(std::array<u8, 16>
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});
constexpr TRAP::Network::IPv6Address TRAP::Network::IPv6Address::LocalHost(std::array<u8, 16>
    {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
	});

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv6Address::IPv6Address(const std::string& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	Resolve(address);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv6Address::IPv6Address(const char* const address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	Resolve(address);
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

[[nodiscard]] TRAP::Network::IPv6Address TRAP::Network::IPv6Address::GetLocalAddress()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//The method here is to connect a UDP socket to anyone (here to localhost),
	//and get the local socket address with the getsockname function.
	//UDP connection will not send anything to the network, so this function won't cause any overhead.

	//Create the socket
	const TRAP::Network::SocketHandle sock = socket(PF_INET6, SOCK_DGRAM, 0);
	if (sock == INTERNAL::Network::SocketImpl::InvalidSocket())
		return {};

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
		return {};
	}

	//Get the local address of the socket connection
	INTERNAL::Network::SocketImpl::AddressLength size = sizeof(address);
	if(getsockname(sock, reinterpret_cast<sockaddr*>(&address), &size) == -1)
	{
		INTERNAL::Network::SocketImpl::Close(sock);
		return {};
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

[[nodiscard]] TRAP::Network::IPv6Address TRAP::Network::IPv6Address::GetPublicAddress(const Utils::TimeStep timeout)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//HTTP URL v6.ident.me
	//GetBody
	TRAP::Network::HTTP server("v6.ident.me");
	const TRAP::Network::HTTP::Request request("", TRAP::Network::HTTP::Request::Method::GET);
	const TRAP::Network::HTTP::Response page = server.SendRequest(request, timeout);
	if(page.GetStatus() == TRAP::Network::HTTP::Response::Status::OK)
		return IPv6Address(page.GetBody());

	//Something failed: return an invalid address
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::IPv6Address::Resolve(const std::string& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	m_address = {};
	m_valid = false;

	if(address.empty())
		return;

	const std::string lowerAddress = Utils::String::ToLower(address);
	if (address == "::" || address == "0:0:0:0:0:0:0:0" ||
		address == "0000:0000:0000:0000:0000:0000:0000:0000")
	{
#ifdef TRAP_PLATFORM_WINDOWS
		std::copy_n(in6addr_any.u.Byte, m_address.size(), m_address.data());
#else
		std::copy_n(in6addr_any.s6_addr, m_address.size(), m_address.data());
#endif
		m_valid = true;
	}
	else
	{
		//Try to convert the address as a hex representation ("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")
		std::array<u8, 16> ip{};
		if(inet_pton(AF_INET6, lowerAddress.c_str(), ip.data()) != 0)
		{
			m_address = ip;
			m_valid = true;
		}
		else
		{
			//Not a valid address, try to convert it as a host name
			addrinfo hints{};
			hints.ai_family = AF_INET6;
			addrinfo* result = nullptr;
			if(getaddrinfo(lowerAddress.c_str(), nullptr, &hints, &result) == 0)
			{
				if(result != nullptr)
				{
					std::copy_n(reinterpret_cast<sockaddr_in6*>(result->ai_addr)->sin6_addr.s6_addr, ip.size(), ip.data());
					freeaddrinfo(result);
					m_address = ip;
					m_valid = true;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::istream& TRAP::Network::operator>>(std::istream& stream, TRAP::Network::IPv6Address& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	std::string str;
	stream >> str;
	address = TRAP::Network::IPv6Address(str);

	return stream;
}
