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

#include "TRAPPCH.h"
#include "UDPSocket.h"

#include "Network/Packet.h"
#include "SocketImpl.h"
#include "Utils/Utils.h"
#include "Utils/Memory.h"

TRAP::Network::UDPSocket::UDPSocket()
	: Socket(Type::UDP), m_buffer(MaxDatagramSize)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint16_t TRAP::Network::UDPSocket::GetLocalPort() const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(GetHandle() == INTERNAL::Network::SocketImpl::InvalidSocket())
		return 0; //We failed to retrieve the port

	//Retrieve information about the local end of the socket
	sockaddr address{};
	INTERNAL::Network::SocketImpl::AddressLength size = sizeof(sockaddr_in);
	if (getsockname(GetHandle(), &address, &size) != -1)
	{
		uint16_t port = Utils::BitCast<sockaddr, sockaddr_in>(address).sin_port;

		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(port);

		return port;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::UDPSocket::Bind(const uint16_t port, const IPv4Address& address)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//Close the socket if it is already bound
	Close();

	//Create the internal socket if it doesn't exist
	CreateIPv4();

	//Check if the address is valid
	if ((address == IPv4Address::None) || (address == IPv4Address::Broadcast))
		return Status::Error;

	//Bind the socket
	const sockaddr_in addr = INTERNAL::Network::SocketImpl::CreateAddress(address.ToInteger(), port);
	const sockaddr finalAddr = Utils::BitCast<sockaddr_in, sockaddr>(addr);
	if(::bind(GetHandle(), &finalAddr, sizeof(addr)) == -1)
	{
		TP_ERROR(Log::NetworkUDPSocketPrefix, "Failed to bind socket to port");
		return Status::Error;
	}

	return Status::Done;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::UDPSocket::Unbind()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//Simply close the socket
	Close();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::UDPSocket::Send(const void* const data, const std::size_t size,
                                                             const IPv4Address& remoteAddress,
														     const uint16_t remotePort)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//Create the internal socket if it doesn't exist
	CreateIPv4();

	//Make sure that all the data will fit in one datagram
	if(size > MaxDatagramSize)
	{
		TP_ERROR(Log::NetworkUDPSocketPrefix, "Can't send data over the network (the number of bytes",
			" to send is greater than TRAP::Network::UDPSocket::MaxDatagramSize)");
		return Status::Error;
	}

	//Build the target address
	const sockaddr_in address = INTERNAL::Network::SocketImpl::CreateAddress(remoteAddress.ToInteger(), remotePort);
	const sockaddr finalAddress = Utils::BitCast<sockaddr_in, sockaddr>(address);

	//Send the data (unlike TCP, all the data is always sent in one call)
	const int64_t sent = sendto(GetHandle(), static_cast<const char*>(data), static_cast<int32_t>(size), 0,
	                            &finalAddress, sizeof(sockaddr_in));

	//Check for errors
	if (sent < 0)
		return INTERNAL::Network::SocketImpl::GetErrorStatus();

	return Status::Done;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::UDPSocket::Receive(void* const data, const std::size_t size,
                                                                std::size_t& received, IPv4Address& remoteAddress,
																uint16_t& remotePort) const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//First clear the variables to fill
	received = 0;
	remoteAddress = IPv4Address();
	remotePort = 0;

	//Check the destination buffer
	if(data == nullptr)
	{
		TP_ERROR(Log::NetworkUDPSocketPrefix,
		         "Can't receive data from the network (the destination buffer is invalid)");
		return Status::Error;
	}

	//Data that will be filled with the other computer's address
	sockaddr_in address = INTERNAL::Network::SocketImpl::CreateAddress(INADDR_ANY, 0);
	sockaddr convertedAddress = Utils::BitCast<sockaddr_in, sockaddr>(address);

	//Receive a chunk of bytes
	INTERNAL::Network::SocketImpl::AddressLength addressSize = sizeof(sockaddr_in);
	const int64_t sizeReceived = recvfrom(GetHandle(), static_cast<char*>(data), static_cast<int32_t>(size), 0,
	                                      &convertedAddress, &addressSize);

	//Check for errors
	if (sizeReceived < 0)
		return INTERNAL::Network::SocketImpl::GetErrorStatus();

	//Fill the sender information
	received = static_cast<std::size_t>(sizeReceived);

	address = Utils::BitCast<sockaddr, sockaddr_in>(convertedAddress);
	uint32_t addr = address.sin_addr.s_addr;
	uint16_t port = address.sin_port;

	if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
	{
		TRAP::Utils::Memory::SwapBytes(addr);
		TRAP::Utils::Memory::SwapBytes(port);
	}

	remoteAddress = IPv4Address(addr);
	remotePort = port;

	return Status::Done;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::UDPSocket::Send(Packet& packet, const IPv4Address& remoteAddress,
                                                             const uint16_t remotePort)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//UDP is a datagram-oriented protocol (as opposed to TCP which is a stream protocol).
	//Sending one datagram is almost safe: it may be lost but if it's received, then its data
	//is guaranteed to be ok.
	//However, splitting a packet into multiple datagrams would be highly
	//unreliable, since datagrams may be reordered, dropped or mixed between different sources.
	//That's why TRAP imposes a limit on packet size so that they can be sent in a single datagram.
	//This also removes the overhead associated to packets -- there's no size to send in addition
	//to the packet's data.

	//Get the data to send from the packet
	std::size_t size = 0;
	const void* const data = packet.OnSend(size);

	//Send it
	return Send(data, size, remoteAddress, remotePort);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::UDPSocket::Receive(Packet& packet, IPv4Address& remoteAddress,
                                                                uint16_t& remotePort)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//See the detailed comment in Send(Packet) above.

	//Receive the datagram
	std::size_t received = 0;
	const Status status = Receive(m_buffer.data(), m_buffer.size(), received, remoteAddress, remotePort);

	//If we received valid data, we can copy it to the user packet
	packet.Clear();
	if ((status == Status::Done) && (received > 0))
		packet.OnReceive(m_buffer.data(), received);

	return status;
}