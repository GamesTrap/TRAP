#ifndef TRAP_NETWORK_TCPSOCKETIPV6_H
#define TRAP_NETWORK_TCPSOCKETIPV6_H

#include <vector>

#include "Socket.h"
#include "Utils/Optional.h"
#include "Utils/Time/TimeStep.h"

namespace TRAP::Network
{
	class IPv6Address;
	class TCPListenerIPv6;
	class Packet;

	/// @brief Specialized socket using the TCP protocol.
	class TCPSocketIPv6 final : public Socket
	{
	public:
		/// @brief Constructor.
		constexpr TCPSocketIPv6() noexcept;

		/// @brief Copy constructor.
		consteval TCPSocketIPv6(const TCPSocketIPv6&) = delete;
		/// @brief Copy assignment operator.
		consteval TCPSocketIPv6& operator=(const TCPSocketIPv6&) = delete;
		/// @brief Move constructor.
		constexpr TCPSocketIPv6(TCPSocketIPv6&&) noexcept = default;
		/// @brief Move assignment operator.
		TCPSocketIPv6& operator=(TCPSocketIPv6&&) noexcept = default;

		/// @brief Destructor.
		~TCPSocketIPv6() override = default;

		/// @brief Get the port to which the socket is bound locally.
		///
		/// If the socket is not connected, this function returns 0.
		/// @return Port to which the socket is bound.
		[[nodiscard]] u16 GetLocalPort() const;

		/// @brief Get the address of the connected peer.
		///
		/// If the socket is not connected, this function returns TRAP::NullOpt.
		/// TRAP::Network::IPv4Address::None.
		/// @return Address of the remote peer.
		[[nodiscard]] TRAP::Optional<IPv6Address> GetRemoteAddress() const;

		/// @brief Get the port of the connected peer to which
		/// the socket is connected.
		///
		/// If the socket is not connected, this function returns 0.
		/// @return Remote port to which the socket is connected.
		[[nodiscard]] u16 GetRemotePort() const;

		/// @brief Connect the socket to a remote peer.
		///
		/// In blocking mode, this function may take a while, especially
		/// if the remote peer is not reachable.
		/// The last parameter allows you to stop trying to connect after a given timeout.
		/// If the socket is already connected, the connection is
		/// forcibly disconnected before attempting to connect again.
		/// @param remoteAddress Address of the remote peer.
		/// @param remotePort Port of the remote peer.
		/// @param timeout Optional maximum time to wait. Default: 2 minutes.
		/// @return Status code.
		[[nodiscard]] Status Connect(const IPv6Address& remoteAddress, u16 remotePort,
		                             Utils::TimeStep timeout = Utils::TimeStep(120.0f));

		/// @brief Disconnect the socket from its remote peer.
		///
		/// This function gracefully closes the connection.
		/// If the socket is not connected, this function has no effect.
		void Disconnect();

		/// @brief Send raw data to the remote peer.
		///
		/// To be able to handle partial sends over non-blocking
		/// sockets, use the Send(const void*, usize, usize&)
		/// overload instead.
		/// This function will fail if the socket is not connected.
		/// @param data Pointer to the sequence of bytes to send.
		/// @param size Number of bytes to send.
		/// @return Status code.
		[[nodiscard]] Status Send(const void* data, usize size) const;

		/// @brief Send raw data to the remote peer.
		///
		/// This function will fail if the socket is not connected
		/// @param data Pointer to the sequence of bytes to send.
		/// @param size Number of bytes to send.
		/// @param sent The number of bytes sent will be written here.
		/// @return Status code.
		[[nodiscard]] Status Send(const void* data, usize size, usize& sent) const;

		/// @brief Receive raw data from the remote peer.
		///
		/// In blocking mode, this function will wait until some
		/// bytes are actually received.
		/// This function will fail if the socket is not connected.
		/// @param data Pointer to the array to fill with the received bytes.
		/// @param size Maximum number of bytes that can be received.
		/// @param received This variable is filled with the actual number of bytes received.
		/// @return Status code.
		[[nodiscard]] Status Receive(void* data, usize size, usize& received) const;

		/// @brief Send a formatted packet of data to the remote peer.
		///
		/// In non-blocking mode, if this function returns TRAP::Network::Socket::Status::Partial,
		/// you must retry sending the same unmodified packet before sending
		/// anything else in order to guarantee the packet arrives at the remote
		/// peer uncorrupted.
		/// This function will fail if the socket is not connected.
		/// @param packet Packet to send.
		/// @return Status code.
		[[nodiscard]] Status Send(Packet& packet);

		/// @brief Receive a formatted packet of data from the remote peer.
		///
		/// In blocking mode, this function will wait until the whole packet
		/// has been received.
		/// This function will fail if the socket is not connected.
		/// @param packet Packet to fill with the received data.
		/// @return Status code.
		[[nodiscard]] Status Receive(Packet& packet);

	private:
		friend class TCPListenerIPv6;

		/// @brief Structure holding the data of a pending packet.
		struct PendingPacket
		{
			u32 Size = 0; //Data of packet size
			usize SizeReceived = 0; //Number of size bytes received so far
			std::vector<u8> Data{}; //Data of the packet
		};

		PendingPacket m_pendingPacket; //Temporary data of the packet currently being received
		std::vector<u8> m_blockToSendBuffer; //Buffer used to prepare data being sent from the socket
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::TCPSocketIPv6::TCPSocketIPv6() noexcept
	: Socket(Type::TCP)
{
}

#endif /*TRAP_NETWORK_TCPSOCKETIPV6_H*/
