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
#include "SocketImplWinAPI.h"

#include "Core/PlatformDetection.h"

#include "Utils/Utils.h"
#include "Utils/ByteSwap.h"

#ifdef TRAP_PLATFORM_WINDOWS

sockaddr_in TRAP::INTERNAL::Network::SocketImpl::CreateAddress(uint32_t address, uint16_t port)
{
	sockaddr_in addr{};
	std::memset(&addr, 0, sizeof(addr));

	if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
	{
		TRAP::Utils::Memory::SwapBytes(address);
		TRAP::Utils::Memory::SwapBytes(port);
	}

	addr.sin_addr.s_addr = address;
	addr.sin_family = AF_INET;
	addr.sin_port = port;

	return addr;
}

//-------------------------------------------------------------------------------------------------------------------//

sockaddr_in6 TRAP::INTERNAL::Network::SocketImpl::CreateAddress(const std::array<uint8_t, 16>& address,
                                                                uint16_t port)
{
	sockaddr_in6 addr{};
	std::memset(&addr, 0, sizeof(addr));
	std::memcpy(addr.sin6_addr.u.Byte, address.data(), address.size());
	addr.sin6_family = AF_INET6;

	if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(port);

	addr.sin6_port = port;

	return addr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::SocketHandle TRAP::INTERNAL::Network::SocketImpl::InvalidSocket()
{
	return INVALID_SOCKET;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::Network::SocketImpl::Close(const TRAP::Network::SocketHandle sock)
{
	closesocket(sock);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::Network::SocketImpl::SetBlocking(const TRAP::Network::SocketHandle sock, const bool block)
{
	u_long blocking = block ? 0 : 1;
	ioctlsocket(sock, FIONBIO, &blocking);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::INTERNAL::Network::SocketImpl::GetErrorStatus()
{
	switch(WSAGetLastError())
	{
	case WSAEWOULDBLOCK:
	case WSAEALREADY:
		return TRAP::Network::Socket::Status::NotReady;

	case WSAECONNABORTED:
	case WSAECONNRESET:
	case WSAETIMEDOUT:
	case WSAENETRESET:
	case WSAENOTCONN:
		return TRAP::Network::Socket::Status::Disconnected;

	case WSAEISCONN:
		return TRAP::Network::Socket::Status::Done; //When connecting a non-blocking socket

	default:
		return TRAP::Network::Socket::Status::Error;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Windows needs some initialization and cleanup to get
/// sockets working properly... so let's create a class that will
/// do it automatically
/// </summary>
struct SocketInitializer
{
	SocketInitializer()
	{
		WSADATA init;
		WSAStartup(MAKEWORD(2, 2), &init);
	}

	~SocketInitializer()
	{
		WSACleanup();
	}

	SocketInitializer(const SocketInitializer&) = default;
	SocketInitializer& operator= (const SocketInitializer&) = default;
	SocketInitializer(SocketInitializer&&) = default;
	SocketInitializer& operator= (SocketInitializer&&) = default;
};

//-------------------------------------------------------------------------------------------------------------------//

SocketInitializer GlobalInitializer;

#endif