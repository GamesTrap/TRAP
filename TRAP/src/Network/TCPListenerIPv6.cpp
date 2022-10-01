/*
////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2022 Laurent Gomila (laurent@sfml-dev.org)
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
#include "TCPListenerIPv6.h"

#include "IP/IPv6Address.h"
#include "Sockets/Socket.h"
#include "Sockets/TCPSocketIPv6.h"
#include "Sockets/SocketImpl.h"
#include "Utils/Utils.h"
#include "Utils/Memory.h"

TRAP::Network::TCPListenerIPv6::TCPListenerIPv6()
	: Socket(Type::TCP)
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

uint16_t TRAP::Network::TCPListenerIPv6::GetLocalPort() const
{
	ZoneScoped;

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

TRAP::Network::Socket::Status TRAP::Network::TCPListenerIPv6::Listen(const uint16_t port, const IPv6Address& address)
{
	ZoneScoped;

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
	ZoneScoped;

	//Simply close the socket
	Socket::Close();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::TCPListenerIPv6::Accept(TCPSocketIPv6& socket) const
{
	ZoneScoped;

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