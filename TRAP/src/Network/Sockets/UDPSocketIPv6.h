#ifndef TRAP_NETWORK_UDPSOCKETIPV6_H
#define TRAP_NETWORK_UDPSOCKETIPV6_H

#include "Socket.h"
#include "Network/IP/IPv6Address.h"

namespace TRAP::Network
{
	class Packet;

	class UDPSocketIPv6 final : public Socket
	{
	public:
		/// <summary>
		/// Constants.
		/// </summary>
		enum
		{
			MaxDatagramSize = 65507 //The maximum number of bytes that can be sent in a single UDP datagram
		};

		/// <summary>
		/// Constructor.
		/// </summary>
		UDPSocketIPv6();

		/// <summary>
		/// Get the port to which the socket is bound locally.
		///
		/// If the socket is not bound to a port, this function returns 0.
		/// </summary>
		/// <returns>Port to which the socket is bound.</returns>
		uint16_t GetLocalPort() const;

		/// <summary>
		/// Bind the socket to a specific port.
		///
		/// Binding the socket to a port is necessary for being
		/// able to receive data on that port.
		/// When providing TRAP::Network::Socket::AnyPort as part, the listener
		/// will request an available port from the system.
		/// The chosen port can be retrieved by calling GetLocalPort().
		///
		/// Since the socket cn only be bound to a single port at
		/// any given moment, if it is already bound when this
		/// function is called, it will be unbound from the previous
		/// port before being bound to the new one.
		/// </summary>
		/// <param name="port">Port to bind the socket to.</param>
		/// <param name="address">Address of the interface to bind to.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Bind(uint16_t port, const IPv6Address& address = IPv6Address::Any);

		/// <summary>
		/// Unbind the socket from the local port to which it is bound.
		///
		/// The port that the socket was previously bound to is immediately
		/// made available to the operating system after this function is called.
		/// This means that a subsequent call to Bind() will be able to re-bind
		/// the port if no other process has done so in the mean time.
		/// If the socket is not bound to a port, this function has no effect.
		/// </summary>
		void Unbind();

		/// <summary>
		/// Send raw data to a remote peer.
		///
		/// Make sure that size is not greater than
		/// UDPSocket::MaxDatagramSize, otherwise this function will
		/// fail and no data will be sent.
		/// </summary>
		/// <param name="data">Pointer to the sequence of bytes to send.</param>
		/// <param name="size">Number of bytes to send.</param>
		/// <param name="remoteAddress">Address of the receiver.</param>
		/// <param name="remotePort">Port of the receiver to send the data to.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Send(const void* data, std::size_t size, const IPv6Address& remoteAddress, uint16_t remotePort);

		/// <summary>
		/// Receive raw data from a remote peer.
		///
		/// In blocking mode, this function will wait until some
		/// bytes are actually received.
		/// Be careful to use a buffer which is large enough for
		/// the data that you intend to receive, if it is too small
		/// then an error will be returned and *all* the data will
		/// be list.
		/// </summary>
		/// <param name="data">Pointer to the array to fill with the received bytes.</param>
		/// <param name="size">Maximum number of bytes that can be received.</param>
		/// <param name="received">This variable is filled with the actual number of bytes received.</param>
		/// <param name="remoteAddress">Address of the peer that sent the data.</param>
		/// <param name="remotePort">Port of the peer that sent the data.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Receive(void* data, std::size_t size, std::size_t& received, IPv6Address& remoteAddress,
		                             uint16_t& remotePort) const;

		/// <summary>
		/// Send a formatted packet of data to a remote peer.
		///
		/// Make sure that the packet size is not greater than
		/// UDPSocket::MaxDatagramSize, otherwise this function will
		/// fail and no data will be sent.
		/// </summary>
		/// <param name="packet">Packet to send.</param>
		/// <param name="remoteAddress">Address of the received.</param>
		/// <param name="remotePort">Port of the receiver to send the data to.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Send(Packet& packet, const IPv6Address& remoteAddress, uint16_t remotePort);

		/// <summary>
		/// Receive a formatted packet of data from a remote peer.
		///
		/// In blocking mode, this function will wait until the whole packet
		/// has been received.
		/// </summary>
		/// <param name="packet">Packet to fill with the received data.</param>
		/// <param name="remoteAddress">Address of the peer that sent the data.</param>
		/// <param name="remotePort">Port of the peer that sent the data.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Receive(Packet& packet, IPv6Address& remoteAddress, uint16_t& remotePort);

	private:
		std::vector<char> m_buffer; //Temporary buffer holding the received data in Receive(Packet)
	};
}

#endif /*TRAP_NETWORK_UDPSOCKETIPV6_H*/