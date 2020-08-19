#ifndef _TRAP_NETWORK_TCPLISTENER_H_
#define _TRAP_NETWORK_TCPLISTENER_H_

#include "IP/IPv4Address.h"
#include "Sockets/Socket.h"

namespace TRAP::Network
{
	class TCPSocket;

	/// <summary>
	/// Socket that listens to new TCP connections
	/// </summary>
	class TCPListener final : public Socket
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		TCPListener();

		/// <summary>
		/// Get the port to which the socket is bound locally.<br>
		/// <br>
		/// If the socket is not listening to a port, this function
		/// returns 0.
		/// </summary>
		/// <returns>Port to which the socket is bound</returns>
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
		/// <param name="port">Port to listen on for incoming connection attempts</param>
		/// <param name="address">Address of the interface to listen on</param>
		/// <returns></returns>
		Status Listen(uint16_t port, const IPv4Address& address = IPv4Address::Any);

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
		/// <param name="socket">Socket that will hold the new connection</param>
		/// <returns>Status code</returns>
		Status Accept(TCPSocket& socket) const;
	};
}

#endif /*_TRAP_NETWORK_TCPLISTENER_H_*/