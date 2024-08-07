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
#include "SocketImplWinAPI.h"

#include "Core/PlatformDetection.h"

#include "Utils/Utils.h"
#include "Utils/Memory.h"

#ifdef TRAP_PLATFORM_WINDOWS

void TRAP::INTERNAL::Network::SocketImpl::Close(const TRAP::Network::SocketHandle sock)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	closesocket(sock);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::Network::SocketImpl::SetBlocking(const TRAP::Network::SocketHandle sock, const bool block)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	u_long blocking = block ? 0 : 1;
	ioctlsocket(sock, FIONBIO, &blocking);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Network::Socket::Status TRAP::INTERNAL::Network::SocketImpl::GetErrorStatus()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	switch(WSAGetLastError())
	{
	case WSAEWOULDBLOCK:
		[[fallthrough]];
	case WSAEALREADY:
		return TRAP::Network::Socket::Status::NotReady;

	case WSAECONNABORTED:
		[[fallthrough]];
	case WSAECONNRESET:
		[[fallthrough]];
	case WSAETIMEDOUT:
		[[fallthrough]];
	case WSAENETRESET:
		[[fallthrough]];
	case WSAENOTCONN:
		return TRAP::Network::Socket::Status::Disconnected;

	case WSAEISCONN:
		return TRAP::Network::Socket::Status::Done; //When connecting a non-blocking socket

	default:
		return TRAP::Network::Socket::Status::Error;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Windows needs some initialization and cleanup to get
/// sockets working properly... so let's create a class that will
/// do it automatically
struct SocketInitializer
{
	SocketInitializer()
	{
		ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

		WSADATA init;
		WSAStartup(MAKEWORD(2, 2), &init);
	}

	~SocketInitializer()
	{
		ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

		WSACleanup();
	}

	constexpr SocketInitializer(const SocketInitializer&) noexcept = delete;
	constexpr SocketInitializer& operator= (const SocketInitializer&) noexcept = delete;
	constexpr SocketInitializer(SocketInitializer&&) noexcept = delete;
	constexpr SocketInitializer& operator= (SocketInitializer&&) noexcept = delete;
};

//-------------------------------------------------------------------------------------------------------------------//

SocketInitializer GlobalInitializer;

#endif /*TRAP_PLATFORM_WINDOWS*/
