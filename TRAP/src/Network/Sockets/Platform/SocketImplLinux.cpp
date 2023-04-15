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
#include "SocketImplLinux.h"

#include "Core/PlatformDetection.h"
#include "Utils/String/String.h"
#include "Utils/Utils.h"
#include "Utils/Memory.h"

#ifdef TRAP_PLATFORM_LINUX

[[nodiscard]] sockaddr_in TRAP::INTERNAL::Network::SocketImpl::CreateAddress(uint32_t address, uint16_t port)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	sockaddr_in addr{};

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

[[nodiscard]] sockaddr_in6 TRAP::INTERNAL::Network::SocketImpl::CreateAddress(const std::array<uint8_t, 16>& address,
                                                                              uint16_t port)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	sockaddr_in6 addr{};
	std::copy(address.begin(), address.end(), addr.sin6_addr.s6_addr);
	addr.sin6_family = AF_INET6;

	if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(port);

	addr.sin6_port = port;

	return addr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Network::SocketHandle TRAP::INTERNAL::Network::SocketImpl::InvalidSocket() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return -1;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::Network::SocketImpl::Close(TRAP::Network::SocketHandle sock)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(::close(sock) < 0)
	{
		TP_ERROR(Log::NetworkSocketUnixPrefix, "Failed to close socket!");
		TP_ERROR(Log::NetworkSocketUnixPrefix, Utils::String::GetStrError());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::Network::SocketImpl::SetBlocking(const TRAP::Network::SocketHandle sock, const bool block)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	const int32_t status = fcntl(sock, F_GETFL);
	if(status < 0)
	{
		TP_ERROR(Log::NetworkSocketUnixPrefix, "Failed to get socket status!");
		TP_ERROR(Log::NetworkSocketUnixPrefix, Utils::String::GetStrError());
		return;
	}

	if (block)
	{
		if (fcntl(sock, F_SETFL, status & ~O_NONBLOCK) < 0)
		{
			TP_ERROR(Log::NetworkSocketUnixPrefix, "Failed to set file status flags");
			TP_ERROR(Log::NetworkSocketUnixPrefix, Utils::String::GetStrError());
		}
	}
	else
	{
		if (fcntl(sock, F_SETFL, status | O_NONBLOCK) < 0)
		{
			TP_ERROR(Log::NetworkSocketPrefix, "Failed to set file status flags");
			TP_ERROR(Log::NetworkSocketPrefix, Utils::String::GetStrError());
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Network::Socket::Status TRAP::INTERNAL::Network::SocketImpl::GetErrorStatus() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	//The followings are sometimes equal to EWOULDBLOCK,
	//so we have to make a special case for them in order
	//to avoid having double values in the switch case
	if ((errno == EAGAIN) || (errno == EINPROGRESS))
		return TRAP::Network::Socket::Status::NotReady;

	switch (errno)
	{
	case EWOULDBLOCK:
		return TRAP::Network::Socket::Status::NotReady;

	case ECONNABORTED:
		[[fallthrough]];
	case ECONNRESET:
		[[fallthrough]];
	case ETIMEDOUT:
		[[fallthrough]];
	case ENETRESET:
		[[fallthrough]];
	case ENOTCONN:
		[[fallthrough]];
	case EPIPE:
		return TRAP::Network::Socket::Status::Disconnected;

	default:
		return TRAP::Network::Socket::Status::Error;
	}
}

#endif /*TRAP_PLATFORM_LINUX*/