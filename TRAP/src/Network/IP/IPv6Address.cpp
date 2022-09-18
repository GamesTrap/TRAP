#include "TRAPPCH.h"
#include "IPv6Address.h"

#include "Core/PlatformDetection.h"
#include "Network/HTTP/HTTP.h"
#include "Network/Sockets/SocketHandle.h"
#include "Network/Sockets/SocketImpl.h"
#include "Utils/String/String.h"

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Network::IPv6Address TRAP::Network::IPv6Address::None{};
const TRAP::Network::IPv6Address TRAP::Network::IPv6Address::Any(std::array<uint8_t, 16>
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});
const TRAP::Network::IPv6Address TRAP::Network::IPv6Address::LocalHost(std::array<uint8_t, 16>
    {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
	});

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv6Address::IPv6Address(const std::string address)
	: m_address(), m_valid(false)
{
	Resolve(std::move(address));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv6Address::IPv6Address(const char* const address)
	: m_address(), m_valid(false)
{
	Resolve(address);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv6Address::IPv6Address(const std::array<uint8_t, 16>& addressBytes)
	: m_address(addressBytes), m_valid(true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Network::IPv6Address::ToString() const
{
	in6_addr address{};
#ifdef TRAP_PLATFORM_WINDOWS
	std::copy(m_address.begin(), m_address.end(), address.u.Byte);
#else
	std::copy(m_address.begin(), m_address.end(), address.s6_addr);
#endif

	//8 * 4 = 8 Blocks 4 Values each | 7 = 7 times ':'
	std::string str(8 * 4 + 7, 0);
	inet_ntop(AF_INET6, &address, str.data(), str.size());
	str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
	return str;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 16> TRAP::Network::IPv6Address::ToArray() const
{
	return m_address;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv6Address TRAP::Network::IPv6Address::GetLocalAddress()
{
	//The method here is to connect a UDP socket to anyone (here to localhost),
	//and get the local socket address with the getsockname function.
	//UDP connection will not send anything to the network, so this function won't cause any overhead.

	//Create the socket
	const TRAP::Network::SocketHandle sock = socket(PF_INET6, SOCK_DGRAM, 0);
	if (sock == INTERNAL::Network::SocketImpl::InvalidSocket())
		return {};

	//Connect the socket to localhost on any port
	std::array<uint8_t, 16> loopback{};
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
	std::array<uint8_t, 16> addr{};
#ifdef TRAP_PLATFORM_WINDOWS
	std::copy_n(address.sin6_addr.u.Byte, addr.size(), addr.data());
#else
	std::copy_n(address.sin6_addr.s6_addr, addr.size(), addr.data());
#endif
	return IPv6Address(addr);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv6Address TRAP::Network::IPv6Address::GetPublicAddress(const Utils::TimeStep timeout)
{
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
		std::array<uint8_t, 16> ip{};
		if(inet_pton(AF_INET6, lowerAddress.c_str(), ip.data()))
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
				if(result)
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

bool TRAP::Network::operator==(const IPv6Address& left, const IPv6Address& right)
{
	return !(left < right) && !(right < left);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator!=(const IPv6Address& left, const IPv6Address& right)
{
	return !(left == right);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator<(const IPv6Address& left, const IPv6Address& right)
{
	return std::make_pair(left.m_valid, left.m_address) < std::make_pair(right.m_valid, right.m_address);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator>(const IPv6Address& left, const IPv6Address& right)
{
	return right < left;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator<=(const IPv6Address& left, const IPv6Address& right)
{
	return !(right < left);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::operator>=(const IPv6Address& left, const IPv6Address& right)
{
	return !(left < right);
}

//-------------------------------------------------------------------------------------------------------------------//

std::istream& TRAP::Network::operator>>(std::istream& stream, IPv6Address& address)
{
	std::string str;
	stream >> str;
	address = IPv6Address(str);

	return stream;
}

//-------------------------------------------------------------------------------------------------------------------//

std::ostream& TRAP::Network::operator<<(std::ostream& stream, const IPv6Address& address)
{
	return stream << address.ToString();
}