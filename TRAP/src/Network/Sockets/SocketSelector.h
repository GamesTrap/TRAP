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

	/// @brief Multiplexer that allows to read form multiple sockets.
	class SocketSelector
	{
	public:
		/// @brief Constructor.
		SocketSelector();

		/// @brief Destructor.
		~SocketSelector() = default;

		/// @brief Copy constructor.
		SocketSelector(const SocketSelector& copy);

		/// @brief Move constructor.
		SocketSelector(SocketSelector&&) noexcept;
		/// @brief Move assignment operator.
		SocketSelector& operator=(SocketSelector&&) noexcept;

		/// @brief Add a new socket to the selector.
		///
		/// This function keeps a weak reference to the socket,
		/// so you have to make sure that the socket is not destroyed
		/// while it is stored in the selector.
		/// This function does nothing if the socket is not valid.
		/// @param socket Reference to the socket to add.
		void Add(Socket& socket);

		/// @brief Remove a socket from the selector.
		///
		/// This function doesn't destroy the socket, it simply
		/// removes the reference that the selector has to it.
		/// @param socket Reference to the socket to remove.
		void Remove(Socket& socket) const;

		/// @brief Remove all the sockets stored in the selector.
		///
		/// This function doesn't destroy any instance, it simply
		/// removes all the references that the selector has to
		/// external sockets.
		void Clear() const;

		/// @brief Wait until one or more sockets are ready to receive.
		///
		/// This function returns as soon as at least one socket has
		/// some data available to be received.
		/// To know which sockets are ready, use the IsReady function.
		/// If you use a timeout and no socket is ready before the timeout
		/// is over, the function returns false.
		/// @param timeout Maximum time to wait, (use TRAP::Utils::TimeStep(0.0f) for infinity).
		/// @return True if there are sockets ready, false otherwise.
		[[nodiscard]] bool Wait(Utils::TimeStep timeout) const;

		/// @brief Test a socket to know if it is ready to receive data.
		///
		/// This function must be used after a call to Wait, to know
		/// which sockets are ready to receive data.
		/// If a socket is ready, a call to receive will never block because we know
		/// that there is data available to read.
		/// Note that if this function returns true for a TCPListener,
		/// this means that it is ready to accept a new connection.
		/// @param socket Socket to test.
		/// @return True if the socket is ready to read, false otherwise.
		[[nodiscard]] bool IsReady(Socket& socket) const;

		/// @brief Overload of assignment operator.
		/// @param right Instance to assign.
		/// @return Reference to self.
		SocketSelector& operator=(const SocketSelector& right);

	private:
		struct SocketSelectorImpl;

		TRAP::Scope<SocketSelectorImpl> m_impl; //Opaque pointer to the implementation (which requires OS-specific types)
	};
}

#endif /*TRAP_NETWORK_SOCKETSELECTOR_H*/