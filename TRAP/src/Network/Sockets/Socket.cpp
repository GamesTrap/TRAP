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
#include "Socket.h"

#include "SocketImpl.h"

TRAP::Network::Socket::Socket(const Type type)
	: m_type(type), m_socket(INTERNAL::Network::SocketImpl::InvalidSocket()), m_isBlocking(true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::~Socket()
{
	//Close the socket before it gets destructed
	Close();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::Socket::SetBlocking(const bool blocking)
{
	//Apply if the socket is already created
	if (m_socket != INTERNAL::Network::SocketImpl::InvalidSocket())
		INTERNAL::Network::SocketImpl::SetBlocking(m_socket, blocking);

	m_isBlocking = blocking;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::Socket::IsBlocking() const
{
	return m_isBlocking;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::SocketHandle TRAP::Network::Socket::GetHandle() const
{
	return m_socket;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::Socket::Create()
{
	//Don't create the socket if it already exists
	if(m_socket == INTERNAL::Network::SocketImpl::InvalidSocket())
	{
		const SocketHandle handle = socket(PF_INET, m_type == Type::TCP ? SOCK_STREAM : SOCK_DGRAM, 0);

		if(handle == INTERNAL::Network::SocketImpl::InvalidSocket())
		{
			TP_ERROR(Log::NetworkSocketPrefix, "Failed to create socket");
			return;
		}

		Create(handle);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::Socket::Create(const SocketHandle handle)
{
	//Don't create the socket if it already exists
	if(m_socket == INTERNAL::Network::SocketImpl::InvalidSocket())
	{
		//Assign the new handle
		m_socket = handle;

		//Set the current blocking state
		SetBlocking(m_isBlocking);

		if(m_type == Type::TCP)
		{
			//Disable the Nagle algorithm (i.e. removes buffering of TCP packets)
			int32_t yes = 1;
			if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1)
				TP_ERROR(Log::NetworkSocketPrefix, "Failed to set socket option \"TCP_NODELAY\"; all your TCP packets will be buffered");
		}
		else
		{
			//Enable broadcast by default for UDP sockets
			int32_t yes = 1;
			if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1)
				TP_ERROR(Log::NetworkSocketPrefix, "Failed to enable broadcast on UDP socket");
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::Socket::Close()
{
	//Close the socket
	if(m_socket != INTERNAL::Network::SocketImpl::InvalidSocket())
	{
		INTERNAL::Network::SocketImpl::Close(m_socket);
		m_socket = INTERNAL::Network::SocketImpl::InvalidSocket();
	}
}
