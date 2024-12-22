#ifndef TRAP_NETWORK_UDPSOCKETIPV6_H
#define TRAP_NETWORK_UDPSOCKETIPV6_H

#include <vector>

#include "Socket.h"
#include "Network/IP/IPv6Address.h"

namespace TRAP::Network
{
	class Packet;

	class UDPSocketIPv6 final : public Socket
	{
	public:
		/// @brief Constants.
		static constexpr usize MaxDatagramSize = 65507u; //The maximum number of bytes that can be sent in a single UDP datagram.

		/// @brief Constructor.
		constexpr UDPSocketIPv6();

		/// @brief Copy constructor.
		consteval UDPSocketIPv6(const UDPSocketIPv6&) = delete;
		/// @brief Copy assignment operator.
		consteval UDPSocketIPv6& operator=(const UDPSocketIPv6&) = delete;
		/// @brief Move constructor.
		constexpr UDPSocketIPv6(UDPSocketIPv6&&) noexcept = default;
		/// @brief Move assignment operator.
		UDPSocketIPv6& operator=(UDPSocketIPv6&&) noexcept = default;

		/// @brief Destructor.
		~UDPSocketIPv6() override = default;

		/// @brief Get the port to which the socket is bound locally.
		///
		/// If the socket is not bound to a port, this function returns 0.
		/// @return Port to which the socket is bound.
		[[nodiscard]] u16 GetLocalPort() const;

		/// @brief Bind the socket to a specific port.
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
		/// @param port Port to bind the socket to.
		/// @param address Address of the interface to bind to.
		/// @return Status code.
		[[nodiscard]] Status Bind(u16 port, const IPv6Address& address = IPv6Address::Any);

		/// @brief Unbind the socket from the local port to which it is bound.
		///
		/// The port that the socket was previously bound to is immediately
		/// made available to the operating system after this function is called.
		/// This means that a subsequent call to Bind() will be able to re-bind
		/// the port if no other process has done so in the mean time.
		/// If the socket is not bound to a port, this function has no effect.
		void Unbind();

		/// @brief Send raw data to a remote peer.
		///
		/// Make sure that size is not greater than
		/// UDPSocket::MaxDatagramSize, otherwise this function will
		/// fail and no data will be sent.
		/// @param data Pointer to the sequence of bytes to send.
		/// @param size Number of bytes to send.
		/// @param remoteAddress Address of the receiver.
		/// @param remotePort Port of the receiver to send the data to.
		/// @return Status code.
		[[nodiscard]] Status Send(const void* data, usize size, const IPv6Address& remoteAddress, u16 remotePort);

		/// @brief Receive raw data from a remote peer.
		///
		/// In blocking mode, this function will wait until some
		/// bytes are actually received.
		/// Be careful to use a buffer which is large enough for
		/// the data that you intend to receive, if it is too small
		/// then an error will be returned and *all* the data will
		/// be list.
		/// @param data Pointer to the array to fill with the received bytes.
		/// @param size Maximum number of bytes that can be received.
		/// @param received This variable is filled with the actual number of bytes received.
		/// @param remoteAddress Address of the peer that sent the data.
		/// @param remotePort Port of the peer that sent the data.
		/// @return Status code.
		[[nodiscard]] Status Receive(void* data, usize size, usize& received, TRAP::Optional<IPv6Address>& remoteAddress,
		                             u16& remotePort) const;

		/// @brief Send a formatted packet of data to a remote peer.
		///
		/// Make sure that the packet size is not greater than
		/// UDPSocket::MaxDatagramSize, otherwise this function will
		/// fail and no data will be sent.
		/// @param packet Packet to send.
		/// @param remoteAddress Address of the received.
		/// @param remotePort Port of the receiver to send the data to.
		/// @return Status code.
		[[nodiscard]] Status Send(Packet& packet, const IPv6Address& remoteAddress, u16 remotePort);

		/// @brief Receive a formatted packet of data from a remote peer.
		///
		/// In blocking mode, this function will wait until the whole packet
		/// has been received.
		/// @param packet Packet to fill with the received data.
		/// @param remoteAddress Address of the peer that sent the data.
		/// @param remotePort Port of the peer that sent the data.
		/// @return Status code.
		[[nodiscard]] Status Receive(Packet& packet, TRAP::Optional<IPv6Address>& remoteAddress, u16& remotePort);

	private:
		std::vector<u8> m_buffer = std::vector<u8>(MaxDatagramSize); //Temporary buffer holding the received data in Receive(Packet).
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::UDPSocketIPv6::UDPSocketIPv6()
	: Socket(Type::UDP)
{
}

#endif /*TRAP_NETWORK_UDPSOCKETIPV6_H*/
