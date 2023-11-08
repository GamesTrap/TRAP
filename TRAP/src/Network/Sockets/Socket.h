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

#ifndef TRAP_NETWORK_SOCKET_H
#define TRAP_NETWORK_SOCKET_H

#include "Core/PlatformDetection.h"

#include "SocketHandle.h"

namespace TRAP::Network
{
	class SocketSelector;

	class Socket
	{
	public:
		/// @brief Constructor.
		constexpr Socket() noexcept = default;
		/// @brief Move constructor.
		constexpr Socket(Socket&&) noexcept = default;
		/// @brief Move assignment operator.
		Socket& operator=(Socket&&) noexcept = default;
		/// @brief Copy constructor.
		consteval Socket(const Socket&) = delete;
		/// @brief Copy assignment operator.
		consteval Socket& operator=(const Socket&) = delete;

		/// @brief Status codes that may be returned by socket functions.
		enum class Status
		{
			Done,         //The socket has sent / received the data
			NotReady,     //The socket is not ready to send / receive data yet
			Partial,      //The socket sent a part of the data
			Disconnected, //The TCP socket has been disconnected
			Error         //An unexpected error happened
		};

		/// @brief Some special values used by sockets.
		enum
		{
			AnyPort = 0 //Special value that tells the system to pick any available port
		};

		/// @brief Destructor.
		virtual ~Socket();

		/// @brief Set the blocking state of the socket.
		///
		/// In blocking mode, calls will not return until they have
		/// completed their task.
		/// For example, a call to receive in
		/// blocking mode won't return until some data was actually
		/// received.
		/// In non-blocking mode, calls will always return immediately,
		/// using the return code to signal whether there was data
		/// available or not.
		/// By default, all sockets are blocking.
		/// @param blocking True to set the socket as blocking, false for non-blocking.
		void SetBlocking(bool blocking);

		/// @brief Tell whether the socket is in blocking or non-blocking mode.
		/// @return True if the socket is blocking, false otherwise.
		[[nodiscard]] constexpr bool IsBlocking() const noexcept;

	protected:
		/// @brief Types of protocols that the socket can use.
		enum class Type
		{
			TCP, //TCP protocol
			UDP  //UDP protocol
		};

		/// @brief Constructor.
		///
		/// This constructor can only be accessed by derived classes.
		/// @param type Type of the socket (TCP or UDP).
		constexpr explicit Socket(Type type) noexcept;

		/// @brief Return the internal handle of the socket.
		///
		/// The returned handle may be invalid if the socket
		/// was not created yet (or already destroyed).
		/// This function can only be accessed by derived classes.
		/// @return The internal (OS-specific) handle of the socket.
		[[nodiscard]] constexpr SocketHandle GetHandle() const noexcept;

		/// @brief Create the internal representation of the socket.
		///
		/// This function can only be accessed by derived classes.
		void CreateIPv4();

		/// @brief Create the internal representation of the socket.
		///
		/// This function can only be accessed by derived classes.
		void CreateIPv6();

		/// @brief Create the internal representation of the socket
		/// from a socket handle.
		///
		/// This function can only be accessed by derived classes.
		/// @param handle OS-specific handle of the socket to wrap.
		void Create(SocketHandle handle);

		/// @brief Close the socket gracefully.
		///
		/// This function can only be accessed by derived classes.
		void Close();

	private:
		friend class SocketSelector;

		Type m_type = Type::TCP; //Type of the socket (TCP or UDP)
#ifdef TRAP_PLATFORM_LINUX
		SocketHandle m_socket = -1; //Socket descriptor
#elif defined(TRAP_PLATFORM_WINDOWS)
		SocketHandle m_socket = INVALID_SOCKET; //Socket descriptor
#endif
		bool m_isBlocking = true; //Current blocking mode of the socket
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Socket::Socket(const Type type) noexcept
	: m_type(type)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Network::Socket::IsBlocking() const noexcept
{
	return m_isBlocking;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Network::SocketHandle TRAP::Network::Socket::GetHandle() const noexcept
{
	return m_socket;
}

#endif /*TRAP_NETWORK_SOCKET_H*/