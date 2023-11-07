#ifndef TRAP_NETWORK_TCPLISTENERIPV6_H
#define TRAP_NETWORK_TCPLISTENERIPV6_H

#include "IP/IPv6Address.h"
#include "Sockets/Socket.h"

namespace TRAP::Network
{
	class TCPSocketIPv6;

	/// @brief Socket that listens to new TCP connections.
	class TCPListenerIPv6 final : public Socket
	{
	public:
		/// @brief Constructor.
		constexpr TCPListenerIPv6() noexcept;

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
		[[nodiscard]] Status Listen(u16 port, const IPv6Address& address = IPv6Address::Any);

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
		[[nodiscard]] Status Accept(TCPSocketIPv6& socket) const;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::TCPListenerIPv6::TCPListenerIPv6() noexcept
	: Socket(Type::TCP)
{
}

#endif /*TRAP_NETWORK_TCPLISTENERIPV6_H*/