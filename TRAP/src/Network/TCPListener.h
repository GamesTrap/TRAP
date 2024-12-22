////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2024 Laurent Gomila (laurent@sfml-dev.org)
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

//Modified by: Jan "GamesTrap" Schuerkamp

#ifndef TRAP_NETWORK_TCPLISTENER_H
#define TRAP_NETWORK_TCPLISTENER_H

#include "IP/IPv4Address.h"
#include "Sockets/Socket.h"

namespace TRAP::Network
{
	class TCPSocket;

	/// @brief Socket that listens to new TCP connections.
	class TCPListener final : public Socket
	{
	public:
		/// @brief Constructor.
		constexpr TCPListener() noexcept;

		/// @brief Copy constructor.
		consteval TCPListener(const TCPListener&) = delete;
		/// @brief Copy assignment operator.
		consteval TCPListener& operator=(const TCPListener&) = delete;
		/// @brief Move constructor.
		constexpr TCPListener(TCPListener&&) noexcept = default;
		/// @brief Move assignment operator.
		TCPListener& operator=(TCPListener&&) noexcept = default;

		/// @brief Destructor.
		~TCPListener() override = default;

		/// @brief Get the port to which the socket is bound locally.
		///
		/// If the socket is not listening to a port, this function returns 0.
		/// @return Port to which the socket is bound.
		[[nodiscard]] u16 GetLocalPort() const;

		/// @brief Start listening for incoming connection attempts.
		///
		/// This function makes the socket start listening on the
		/// specified port, waiting for incoming connection attempts.
		///
		/// If the socket is already listening on a port when this
		/// function is called, it will stop listening on the old
		/// port before starting to listen on the new port.
		///
		/// When providing TRAP::Network::Socket::AnyPort as port, the listener
		/// will request an available port from the system.
		/// The chosen port can be retrieved by calling GetLocalPort().
		/// @param port Port to listen on for incoming connection attempts.
		/// @param address Address of the interface to listen on.
		/// @return Status code.
		[[nodiscard]] Status Listen(u16 port, const IPv4Address& address = IPv4Address::Any);

		/// @brief Stop listening and close the socket.
		///
		/// This function gracefully stops the listener.
		/// If the socket is not listening, this function has no effect.
		void Close();

		/// @brief Accept a new connection.
		///
		/// If the socket is in blocking mode, this function will
		/// not return until a connection is actually received.
		/// @param socket Socket that will hold the new connection.
		/// @return Status code.
		[[nodiscard]] Status Accept(TCPSocket& socket) const;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::TCPListener::TCPListener() noexcept
	: Socket(Type::TCP)
{
}

#endif /*TRAP_NETWORK_TCPLISTENER_H*/
