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

#ifndef _TRAP_NETWORK_SOCKETIMPLWINAPI_H_
#define _TRAP_NETWORK_SOCKETIMPLWINAPI_H_

#include "Network/Sockets/Socket.h"
#include "Network/Sockets/SocketHandle.h"

#ifdef TRAP_PLATFORM_WINDOWS

namespace TRAP::INTERNAL::Network
{
	/// <summary>
	/// Helper class implementing all the non-portable
	/// socket stuff; this is the Windows version
	/// </summary>
	class SocketImpl
	{
	public:
		/// <summary>
		/// Types
		/// </summary>
		typedef int32_t AddressLength;

		/// <summary>
		/// Create an internal sockaddr_in address
		/// </summary>
		/// <param name="address">Target address</param>
		/// <param name="port">Target port</param>
		/// <returns>sockaddr_in ready to be used by socket functions</returns>
		static sockaddr_in CreateAddress(uint32_t address, uint16_t port);

		/// <summary>
		/// Return the value of the invalid socket
		/// </summary>
		/// <returns>Special value of the invalid socket</returns>
		static TRAP::Network::SocketHandle InvalidSocket();

		/// <summary>
		/// Close and destroy a socket
		/// </summary>
		/// <param name="sock">Handle of the socket to close</param>
		static void Close(TRAP::Network::SocketHandle sock);

		/// <summary>
		/// Set a socket as blocking or non-blocking
		/// </summary>
		/// <param name="sock">Handle of the socket</param>
		/// <param name="block">New blocking state of the socket</param>
		static void SetBlocking(TRAP::Network::SocketHandle sock, bool block);

		/// <summary>
		/// Get the last socket error status
		/// </summary>
		/// <returns>Status corresponding to the last socket error</returns>
		static TRAP::Network::Socket::Status GetErrorStatus();
	};
}

#endif

#endif /*_TRAP_NETWORK_SOCKETIMPLWINAPI_H_*/