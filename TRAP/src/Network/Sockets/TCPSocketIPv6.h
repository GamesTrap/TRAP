#ifndef TRAP_NETWORK_TCPSOCKETIPV6_H
#define TRAP_NETWORK_TCPSOCKETIPV6_H

#include <vector>

#include "Socket.h"
#include "Utils/Time/TimeStep.h"

namespace TRAP::Network
{
	class IPv6Address;
	class TCPListenerIPv6;
	class Packet;

	/// <summary>
	/// Specialized socket using the TCP protocol.
	/// </summary>
	class TCPSocketIPv6 final : public Socket
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		constexpr TCPSocketIPv6() noexcept;

		/// <summary>
		/// Get the port to which the socket is bound locally.
		///
		/// If the socket is not connected, this function returns 0.
		/// </summary>
		/// <returns>Port to which the socket is bound.</returns>
		[[nodiscard]] uint16_t GetLocalPort() const;

		/// <summary>
		/// Get the address of the connected peer.
		///
		/// If the socket is not connected, this function returns
		/// TRAP::Network::IPv4Address::None.
		/// </summary>
		/// <returns>Address of the remote peer.</returns>
		[[nodiscard]] IPv6Address GetRemoteAddress() const;

		/// <summary>
		/// Get the port of the connected peer to which
		/// the socket is connected.
		///
		/// If the socket is not connected, this function returns 0.
		/// </summary>
		/// <returns>Remote port to which the socket is connected.</returns>
		[[nodiscard]] uint16_t GetRemotePort() const;

		/// <summary>
		/// Connect the socket to a remote peer.
		///
		/// In blocking mode, this function may take a while, especially
		/// if the remote peer is not reachable.
		/// The last parameter allows you to stop trying to connect after a given timeout.
		/// If the socket is already connected, the connection is
		/// forcibly disconnected before attempting to connect again.
		/// </summary>
		/// <param name="remoteAddress">Address of the remote peer.</param>
		/// <param name="remotePort">Port of the remote peer.</param>
		/// <param name="timeout">Optional maximum time to wait.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Connect(const IPv6Address& remoteAddress, uint16_t remotePort,
		                             Utils::TimeStep timeout = Utils::TimeStep(0.0f));

		/// <summary>
		/// Disconnect the socket from its remote peer.
		///
		/// This function gracefully closes the connection.
		/// If the socket is not connected, this function has no effect.
		/// </summary>
		void Disconnect();

		/// <summary>
		/// Send raw data to the remote peer.
		///
		/// To be able to handle partial sends over non-blocking
		/// sockets, use the Send(const void*, std::size_t, std::size_t&)
		/// overload instead.
		/// This function will fail if the socket is not connected.
		/// </summary>
		/// <param name="data">Pointer to the sequence of bytes to send.</param>
		/// <param name="size">Number of bytes to send.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Send(const void* data, std::size_t size) const;

		/// <summary>
		/// Send raw data to the remote peer.
		///
		/// This function will fail if the socket is not connected
		/// </summary>
		/// <param name="data">Pointer to the sequence of bytes to send.</param>
		/// <param name="size">Number of bytes to send.</param>
		/// <param name="sent">The number of bytes sent will be written here.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Send(const void* data, std::size_t size, std::size_t& sent) const;

		/// <summary>
		/// Receive raw data from the remote peer.
		///
		/// In blocking mode, this function will wait until some
		/// bytes are actually received.
		/// This function will fail if the socket is not connected.
		/// </summary>
		/// <param name="data">Pointer to the array to fill with the received bytes.</param>
		/// <param name="size">Maximum number of bytes that can be received.</param>
		/// <param name="received">This variable is filled with the actual number of bytes received.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Receive(void* data, std::size_t size, std::size_t& received) const;

		/// <summary>
		/// Send a formatted packet of data to the remote peer.
		///
		/// In non-blocking mode, if this function returns TRAP::Network::Socket::Status::Partial,
		/// you must retry sending the same unmodified packet before sending
		/// anything else in order to guarantee the packet arrives at the remote
		/// peer uncorrupted.
		/// This function will fail if the socket is not connected.
		/// </summary>
		/// <param name="packet">Packet to send.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Send(Packet& packet) const;

		/// <summary>
		/// Receive a formatted packet of data from the remote peer.
		///
		/// In blocking mode, this function will wait until the whole packet
		/// has been received.
		/// This function will fail if the socket is not connected.
		/// </summary>
		/// <param name="packet">Packet to fill with the received data.</param>
		/// <returns>Status code.</returns>
		[[nodiscard]] Status Receive(Packet& packet);

	private:
		friend class TCPListenerIPv6;

		/// <summary>
		/// Structure holding the data of a pending packet.
		/// </summary>
		struct PendingPacket
		{
			uint32_t Size = 0; //Data of packet size
			std::size_t SizeReceived = 0; //Number of size bytes received so far
			std::vector<uint8_t> Data{}; //Data of the packet
		};

		PendingPacket m_pendingPacket; //Temporary data of the packet currently being received
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::TCPSocketIPv6::TCPSocketIPv6() noexcept
	: Socket(Type::TCP)
{
}

#endif /*TRAP_NETWORK_TCPSOCKETIPV6_H*/