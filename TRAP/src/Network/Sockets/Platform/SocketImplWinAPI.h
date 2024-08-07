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

#ifndef TRAP_NETWORK_SOCKETIMPLWINAPI_H
#define TRAP_NETWORK_SOCKETIMPLWINAPI_H

#include "Core/PlatformDetection.h"
#include "Core/Types.h"
#include "Network/Sockets/Socket.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"
#include "Network/Sockets/SocketHandle.h"

#ifdef TRAP_PLATFORM_WINDOWS

/// @brief Helper namespace implementing all the non-portable
/// socket stuff; this is the Windows version.
namespace TRAP::INTERNAL::Network::SocketImpl
{
	/// @brief Types.
	using AddressLength = i32;

	/// @brief Create an internal sockaddr_in address.
	/// @param address Target address.
	/// @param port Target port.
	/// @return sockaddr_in ready to be used by socket functions.
	[[nodiscard]] constexpr sockaddr_in CreateAddress(u32 address, u16 port);

	/// @brief Create an internal sockaddr_in6 address.
	/// @param address Target address.
	/// @param port Target port.
	/// @return sockaddr_in6 ready to be used by socket functions.
	[[nodiscard]] constexpr sockaddr_in6 CreateAddress(const std::array<u8, 16>& address, u16 port);

	/// @brief Return the value of the invalid socket.
	/// @return Special value of the invalid socket.
	[[nodiscard]] constexpr TRAP::Network::SocketHandle InvalidSocket() noexcept;

	/// @brief Close and destroy a socket.
	/// @param sock Handle of the socket to close.
	void Close(TRAP::Network::SocketHandle sock);

	/// @brief Set a socket as blocking or non-blocking.
	/// @param sock Handle of the socket.
	/// @param block New blocking state of the socket.
	void SetBlocking(TRAP::Network::SocketHandle sock, bool block);

	/// @brief Get the last socket error status.
	/// @return Status corresponding to the last socket error.
	[[nodiscard]] TRAP::Network::Socket::Status GetErrorStatus();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr sockaddr_in TRAP::INTERNAL::Network::SocketImpl::CreateAddress(u32 address, u16 port)
{
	sockaddr_in addr{};

	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
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

[[nodiscard]] constexpr sockaddr_in6 TRAP::INTERNAL::Network::SocketImpl::CreateAddress(const std::array<u8, 16>& address,
                                                                                        u16 port)
{
	sockaddr_in6 addr{};
	std::ranges::copy(address, addr.sin6_addr.u.Byte);
	addr.sin6_family = AF_INET6;

	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
	{
		TRAP::Utils::Memory::SwapBytes(port);
	}

	addr.sin6_port = port;

	return addr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Network::SocketHandle TRAP::INTERNAL::Network::SocketImpl::InvalidSocket() noexcept
{
	return INVALID_SOCKET;
}

#endif /*TRAP_PLATFORM_WINDOWS*/

#endif /*TRAP_NETWORK_SOCKETIMPLWINAPI_H*/
