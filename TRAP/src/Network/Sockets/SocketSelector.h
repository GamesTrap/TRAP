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

#ifndef TRAP_NETWORK_SOCKETSELECTOR_H
#define TRAP_NETWORK_SOCKETSELECTOR_H

#include "Core/Base.h"

namespace TRAP::Utils
{
	struct TimeStep;
}

namespace TRAP::Network
{
	class Socket;

	/// <summary>
	/// Multiplexer that allows to read form multiple sockets.
	/// </summary>
	class SocketSelector
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		SocketSelector();

		/// <summary>
		/// Destructor.
		/// </summary>
		~SocketSelector();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		SocketSelector(const SocketSelector& copy);

		/// <summary>
		/// Move constructor.
		/// </summary>
		SocketSelector(SocketSelector&&) noexcept;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		SocketSelector& operator=(SocketSelector&&) noexcept;

		/// <summary>
		/// Add a new socket to the selector.
		///
		/// This function keeps a weak reference to the socket,
		/// so you have to make sure that the socket is not destroyed
		/// while it is stored in the selector.
		/// This function does nothing if the socket is not valid.
		/// </summary>
		/// <param name="socket">Reference to the socket to add.</param>
		void Add(Socket& socket);

		/// <summary>
		/// Remove a socket from the selector.
		///
		/// This function doesn't destroy the socket, it simply
		/// removes the reference that the selector has to it.
		/// </summary>
		/// <param name="socket">Reference to the socket to remove.</param>
		void Remove(Socket& socket) const;

		/// <summary>
		/// Remove all the sockets stored in the selector.
		///
		/// This function doesn't destroy any instance, it simply
		/// removes all the references that the selector has to
		/// external sockets.
		/// </summary>
		void Clear() const;

		/// <summary>
		/// Wait until one or more sockets are ready to receive.
		///
		/// This function returns as soon as at least one socket has
		/// some data available to be received.
		/// To know which sockets are ready, use the IsReady function.
		/// If you use a timeout and no socket is ready before the timeout
		/// is over, the function returns false.
		/// </summary>
		/// <param name="timeout">Maximum time to wait, (use TRAP::Utils::TimeStep(0.0f) for infinity).</param>
		/// <returns>True if there are sockets ready, false otherwise.</returns>
		[[nodiscard]] bool Wait(Utils::TimeStep timeout) const;

		/// <summary>
		/// Test a socket to know if it is ready to receive data.
		///
		/// This function must be used after a call to Wait, to know
		/// which sockets are ready to receive data.
		/// If a socket is ready, a call to receive will never block because we know
		/// that there is data available to read.
		/// Note that if this function returns true for a TCPListener,
		/// this means that it is ready to accept a new connection.
		/// </summary>
		/// <param name="socket">Socket to test.</param>
		/// <returns>True if the socket is ready to read, false otherwise.</returns>
		bool IsReady(Socket& socket) const;

		/// <summary>
		/// Overload of assignment operator.
		/// </summary>
		/// <param name="right">Instance to assign.</param>
		/// <returns>Reference to self.</returns>
		SocketSelector& operator=(const SocketSelector& right);

	private:
		struct SocketSelectorImpl;

		TRAP::Scope<SocketSelectorImpl> m_impl; //Opaque pointer to the implementation (which requires OS-specific types)
	};
}

#endif /*TRAP_NETWORK_SOCKETSELECTOR_H*/