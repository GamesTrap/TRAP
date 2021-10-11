#ifndef _TRAP_NETWORK_TCPLISTENERIPV6_H_
#define _TRAP_NETWORK_TCPLISTENERIPV6_H_

#include "IP/IPv6Address.h"
#include "Sockets/Socket.h"

namespace TRAP::Network
{
	class TCPSocketIPv6;

	/// <summary>
	/// Socket that listens to new TCP connections.
	/// </summary>
	class TCPListenerIPv6 final : public Socket
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		TCPListenerIPv6();
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~TCPListenerIPv6() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		TCPListenerIPv6(const TCPListenerIPv6&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		TCPListenerIPv6& operator=(const TCPListenerIPv6&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		TCPListenerIPv6(TCPListenerIPv6&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		TCPListenerIPv6& operator=(TCPListenerIPv6&&) = default;

		/// <summary>
		/// Get the port to which the socket is bound locally.<br>
		/// <br>
		/// If the socket is not listening to a port, this function returns 0.
		/// </summary>
		/// <returns>Port to which the socket is bound.</returns>
		uint16_t GetLocalPort() const;

		/// <summary>
		/// Start listening for incoming connection attempts.<br>
		/// <br>
		/// This function makes the socket start listening on the
		/// specified port, waiting for incoming connection attempts.<br>
		/// <br>
		/// If the socket is already listening on a port when this
		/// function is called, it will stop listening on the old
		/// port before starting to listen on the new port.<br>
		/// <br>
		/// When providing TRAP::Network::Socket::AnyPort as port, the listener
		/// will request an available port from the system.<b>
		/// The chosen port can be retrieved by calling GetLocalPort().
		/// </summary>
		/// <param name="port">Port to listen on for incoming connection attempts.</param>
		/// <param name="address">Address of the interface to listen on.</param>
		/// <returns>Status code.</returns>
		Status Listen(uint16_t port, const IPv6Address& address = IPv6Address::Any);

		/// <summary>
		/// Stop listening and close the socket.<br>
		/// <br>
		/// This function gracefully stops the listener.<br>
		/// If the socket is not listening, this function has no effect.
		/// </summary>
		void Close();

		/// <summary>
		/// Accept a new connection.<br>
		/// <br>
		/// If the socket is in blocking mode, this function will
		/// not return until a connection is actually received.
		/// </summary>
		/// <param name="socket">Socket that will hold the new connection.</param>
		/// <returns>Status code.</returns>
		Status Accept(TCPSocketIPv6& socket) const;
	};
}

#endif /*_TRAP_NETWORK_TCPLISTENERIPV6_H_*/