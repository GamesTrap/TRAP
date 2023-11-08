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

#ifndef TRAP_NETWORK_TCPSOCKET_H
#define TRAP_NETWORK_TCPSOCKET_H

#include <vector>

#include "Socket.h"
#include "Utils/Time/TimeStep.h"

namespace TRAP::Network
{
	class TCPListener;
	class IPv4Address;
	class Packet;

	/// @brief Specialized socket using the TCP protocol.
	class TCPSocket final : public Socket
	{
	public:
		/// @brief Constructor.
		constexpr TCPSocket() noexcept;

		/// @brief Copy constructor.
		consteval TCPSocket(const TCPSocket&) = delete;
		/// @brief Copy assignment operator.
		consteval TCPSocket& operator=(const TCPSocket&) = delete;
		/// @brief Move constructor.
		constexpr TCPSocket(TCPSocket&&) noexcept = default;
		/// @brief Move assignment operator.
		TCPSocket& operator=(TCPSocket&&) noexcept = default;

		/// @brief Destructor.
		~TCPSocket() override = default;

		/// @brief Get the port to which the socket is bound locally.
		///
		/// If the socket is not connected, this function returns 0.
		/// @return Port to which the socket is bound.
		[[nodiscard]] u16 GetLocalPort() const;

		/// @brief Get the address of the connected peer.
		///
		/// If the socket is not connected, this function returns
		/// TRAP::Network::IPv4Address::None.
		/// @return Address of the remote peer.
		[[nodiscard]] IPv4Address GetRemoteAddress() const;

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
		/// @param timeout Optional maximum time to wait.
		/// @return Status code.
		[[nodiscard]] Status Connect(const IPv4Address& remoteAddress, u16 remotePort,
		                             Utils::TimeStep timeout = Utils::TimeStep(0.0f));

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
		/// This function will fail if the socket is not connected
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

		/// @brief Send a formatted packet of adata to the remote peer.
		///
		/// In non-blocking mode, if this function returns TRAP::Network::Socket::Status::Partial,
		/// you must retry sending the same unmodified packet before sending
		/// anything else in order to guarantee the packet arrives at the remote
		/// peer uncorrupted.
		/// This function will fail if the socket is not connected.
		/// @param packet Packet to send.
		/// @return Status code.
		[[nodiscard]] Status Send(Packet& packet) const;

		/// @brief Receive a formatted packet of data from the remote peer.
		///
		/// In blocking mode, this function will wait until the whole packet
		/// has been received.
		/// This function will fail if the socket is not connected.
		/// @param packet Packet to fill with the received data.
		/// @return Status code.
		[[nodiscard]] Status Receive(Packet& packet);

	private:
		friend class TCPListener;

		/// @brief Structure holding the data of a pending packet.
		struct PendingPacket
		{
			u32 Size = 0;            //Data of packet size
			usize SizeReceived = 0; //Number of size bytes received so far
			std::vector<u8> Data{};   //Data of the packet
		};

		PendingPacket m_pendingPacket; //Temporary data of the packet currently being received
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::TCPSocket::TCPSocket() noexcept
	: Socket(Type::TCP)
{
}

#endif /*TRAP_NETWORK_TCPSOCKET_H*/