#include "TRAPPCH.h"
#include "TCPListenerIPv6.h"

#include "IP/IPv6Address.h"
#include "Sockets/Socket.h"
#include "Sockets/TCPSocketIPv6.h"
#include "Sockets/SocketImpl.h"
#include "Utils/Utils.h"
#include "Utils/Memory.h"

[[nodiscard]] uint16_t TRAP::Network::TCPListenerIPv6::GetLocalPort() const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(GetHandle() == INTERNAL::Network::SocketImpl::InvalidSocket())
		return 0; //We failed to retrieve the port

	//Retrieve information about the local end of the socket
	sockaddr_in6 address{};
	INTERNAL::Network::SocketImpl::AddressLength size = sizeof(sockaddr_in6);
	if (getsockname(GetHandle(), reinterpret_cast<sockaddr*>(&address), &size) != -1)
	{
		uint16_t res = address.sin6_port;

		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(res);

		return res;
	}

	return 0; //We failed to retrieve the port
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Network::Socket::Status TRAP::Network::TCPListenerIPv6::Listen(const uint16_t port, const IPv6Address& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//Close the socket if it is already bound
	Close();

	//Create the internal socket if it doesn't exist
	CreateIPv6();

	//Check if the address is valid
	if (address == IPv6Address::None)
		return Status::Error;

	//Bind the socket to the specified port
	const sockaddr_in6 addr = INTERNAL::Network::SocketImpl::CreateAddress(address.ToArray(), port);
	if(bind(GetHandle(), reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in6)) == -1)
	{
		//Not likely to happen, but...
		TP_ERROR(Log::NetworkTCPListenerPrefix, "Failed to bind listener socket to port", port);
		return Status::Error;
	}

	//Listen to the bound port
	if(::listen(GetHandle(), SOMAXCONN) == -1)
	{
		//Oops, socket is deaf
		TP_ERROR(Log::NetworkTCPListenerPrefix, "Failed to listen to port", port);
		return Status::Error;
	}

	return Status::Done;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::TCPListenerIPv6::Close()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//Simply close the socket
	Socket::Close();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Network::Socket::Status TRAP::Network::TCPListenerIPv6::Accept(TCPSocketIPv6& socket) const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//Make sure that we're listening
	if(GetHandle() == INTERNAL::Network::SocketImpl::InvalidSocket())
	{
		TP_ERROR(Log::NetworkTCPListenerPrefix, "Failed to accept a new connection, the socket is not listening");
		return Status::Error;
	}

	//Accept a new connection
	sockaddr_in6 address{};
	INTERNAL::Network::SocketImpl::AddressLength length = sizeof(sockaddr_in6);
	const SocketHandle remote = ::accept(GetHandle(), reinterpret_cast<sockaddr*>(&address), &length);

	//Check for errors
	if (remote == INTERNAL::Network::SocketImpl::InvalidSocket())
		return INTERNAL::Network::SocketImpl::GetErrorStatus();

	//Initialize the new connected socket
	socket.Close();
	socket.Create(remote);

	return Status::Done;
}