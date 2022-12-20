/*
////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2022 Laurent Gomila (laurent@sfml-dev.org)
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
#include "TCPSocket.h"

#include "Core/PlatformDetection.h"
#include "Network/Packet.h"
#include "Network/IP/IPv4Address.h"
#include "SocketImpl.h"
#include "Utils/Utils.h"
#include "Utils/Memory.h"

#ifdef TRAP_PLATFORM_WINDOWS
#define far
#endif

namespace
{
	//Define the low-level send/receive flags, which depend on the OS
#ifdef TRAP_PLATFORM_LINUX
	const int32_t flags = MSG_NOSIGNAL;
#else
	const int32_t flags = 0;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::TCPSocket::TCPSocket() noexcept
	: Socket(Type::TCP)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);
}

//-------------------------------------------------------------------------------------------------------------------//

uint16_t TRAP::Network::TCPSocket::GetLocalPort() const
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

	//Failed to retrieve the port
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv4Address TRAP::Network::TCPSocket::GetRemoteAddress() const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(GetHandle() == INTERNAL::Network::SocketImpl::InvalidSocket())
		return IPv4Address::None; //We failed to retrieve the address

	//Retrieve information about the remote end of the socket
	sockaddr address{};
	INTERNAL::Network::SocketImpl::AddressLength size = sizeof(sockaddr_in);
	if (getpeername(GetHandle(), &address, &size) != -1)
	{
		uint32_t addr = Utils::BitCast<sockaddr, sockaddr_in>(address).sin_addr.s_addr;

		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(addr);

		return IPv4Address(addr);
	}

	return IPv4Address::None;
}

//-------------------------------------------------------------------------------------------------------------------//

uint16_t TRAP::Network::TCPSocket::GetRemotePort() const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(GetHandle() == INTERNAL::Network::SocketImpl::InvalidSocket())
		return 0; //We failed to retrieve the port

	//Retrieve information about the remote end of the socket
	sockaddr address{};
	INTERNAL::Network::SocketImpl::AddressLength size = sizeof(sockaddr_in);
	if (getpeername(GetHandle(), &address, &size) != -1)
	{
		uint16_t port = Utils::BitCast<sockaddr, sockaddr_in>(address).sin_port;

		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(port);

		return port;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::TCPSocket::Connect(const IPv4Address& remoteAddress,
                                                                const uint16_t remotePort, Utils::TimeStep timeout)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//Disconnect the socket if it is already connected
	Disconnect();

	//Create the internal socket if it doesn't exist
	CreateIPv4();

	//Create the remote address
	const sockaddr_in address = INTERNAL::Network::SocketImpl::CreateAddress(remoteAddress.ToInteger(), remotePort);
	const sockaddr convertedAddr = Utils::BitCast<sockaddr_in, sockaddr>(address);

	if(timeout <= Utils::TimeStep(0.0f))
	{
		//We're not using a timeout: just try to connect

		//Connect the socket
		if (::connect(GetHandle(), &convertedAddr, sizeof(sockaddr_in)) == -1)
			return INTERNAL::Network::SocketImpl::GetErrorStatus();

		//Connection succeeded
		return Status::Done;
	}

	//We're using a timeout: we'll need a few tricks to make it work

	//save the previous blocking state
	const bool blocking = IsBlocking();

	//Switch to non-blocking to enable our connection timeout
	if (blocking)
		SetBlocking(false);

	//Try to connect to the remote address
	if(::connect(GetHandle(), &convertedAddr, sizeof(sockaddr_in)) >= 0)
	{
		//We got instantly connected! (it may no happen a lot...)
		SetBlocking(blocking);
		return Status::Done;
	}

	//Get the error status
	Status status = INTERNAL::Network::SocketImpl::GetErrorStatus();

	//If we were in non-blocking mode, return immediately
	if (!blocking)
		return status;

	//Otherwise, wait until something happens to our socket (success, timeout or error)
	if(status == Status::NotReady)
	{
		//Setup the selector
		fd_set selector;
		FD_ZERO(&selector);
		FD_SET(GetHandle(), &selector);

		//Setup the timeout
		timeval time{};
		time.tv_sec = static_cast<time_t>(timeout.GetSeconds());
		time.tv_usec = static_cast<time_t>(timeout.GetSeconds());

		//Wait for something to write on our socket (which means that the connection request has returned)
		if(select(static_cast<int>(GetHandle() + 1), nullptr, &selector, nullptr, &time) > 0)
		{
			//At this point the connection may have been either accepted or refused.
			//To know whether it's a success or a failure, we must check the address of the connected peer
			if(GetRemoteAddress() != IPv4Address::None)
				status = Status::Done; //Connection accepted
			else
				status = INTERNAL::Network::SocketImpl::GetErrorStatus(); //Conncetion refused
		}
		else
			status = INTERNAL::Network::SocketImpl::GetErrorStatus(); //Failed to connect before timeout is over
	}

	//Switch back to blocking mode
	SetBlocking(true);

	return status;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::TCPSocket::Disconnect()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//Close the socket
	Close();

	//Reset the pending packet data
	m_pendingPacket = PendingPacket();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::TCPSocket::Send(const void* const data, const std::size_t size) const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if (!IsBlocking())
		TP_WARN(Log::NetworkTCPSocketPrefix, "Partial sends might not be handled properly.");

	std::size_t sent = 0;

	return Send(data, size, sent);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::TCPSocket::Send(const void* const data, const std::size_t size,
                                                             std::size_t& sent) const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//Check the parameters
	if(!data || (size == 0))
	{
		TP_ERROR(Log::NetworkTCPSocketPrefix, "Can't send data over the network (no data to send)");
		return Status::Error;
	}

	//Loop until every byte has been sent
	int64_t result = 0;
	for(sent = 0; sent < size; sent += result)
	{
		//Send a chunk of data
		result = ::send(GetHandle(), static_cast<const char*>(data) + sent, static_cast<int>(size - sent), flags);

		//Check for errors
		if(result < 0)
		{
			const Status status = INTERNAL::Network::SocketImpl::GetErrorStatus();

			if ((status == Status::NotReady) && sent)
				return Status::Partial;

			return status;
		}
	}

	return Status::Done;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::TCPSocket::Receive(void* const data, const std::size_t size,
                                                                std::size_t& received) const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//First clear the variables to fill
	received = 0;

	//Check the destination buffer
	if(!data)
	{
		TP_ERROR(Log::NetworkTCPSocketPrefix,
		         "Can't receive data from the network (the destination buffer is invalid)");
		return Status::Error;
	}

	//Receive a chunk of bytes
	const int64_t sizeReceived = recv(GetHandle(), static_cast<char*>(data), static_cast<int>(size), flags);

	//Check the number of bytes received
	if (sizeReceived > 0)
	{
		received = static_cast<std::size_t>(sizeReceived);
		return Status::Done;
	}
	if (sizeReceived == 0)
		return Status::Disconnected;

	return INTERNAL::Network::SocketImpl::GetErrorStatus();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::TCPSocket::Send(Packet& packet) const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//TCP is a stream protocol, it doesn't preserve messages boundaries.
	//This means that we have to send the packet size first, so that the
	//receiver knows the actual end of the packet in the data stream.

	//We allocate an extra memory block so that the size can be sent
	//together with the data in a single call.
	//This may seem inefficient, but is is actually required to avoid
	//partial send, which could cause data corruption on the receiving end.

	//Get the data to send from the packet
	std::size_t size = 0;
	const void* const data = packet.OnSend(size);

	//First convert the packet size to network byte order
	uint32_t packetSize = static_cast<uint32_t>(size);

	if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(packetSize);

	//Allocate memory for the data block to send
	std::vector<char> blockToSend(sizeof(packetSize) + size);

	//Copy the packet size and data into the block to send
	std::copy_n(reinterpret_cast<const uint8_t*>(&packetSize), sizeof(packetSize), blockToSend.data());
	if (size > 0)
		std::copy_n(static_cast<const uint8_t*>(data), size, blockToSend.data() + sizeof(packetSize));

	//Send the data block
	std::size_t sent = 0;
	const Status status = Send(&blockToSend[0] + packet.m_sendPos, blockToSend.size() - packet.m_sendPos, sent);

	//In the case of a partial send, record the location to resume from
	if (status == Status::Partial)
		packet.m_sendPos += sent;
	else if (status == Status::Done)
		packet.m_sendPos = 0;

	return status;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::TCPSocket::Receive(Packet& packet)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//First clear the variables to fill
	packet.Clear();

	//We start by getting the size of the incoming packet
	uint32_t packetSize = 0;
	std::size_t received = 0;
	if(m_pendingPacket.SizeReceived < sizeof(m_pendingPacket.Size))
	{
		//Loop until we've received the entire size of the packet
		//(even a 4 byte variable may be received in more than one call)
		while(m_pendingPacket.SizeReceived < sizeof(m_pendingPacket.Size))
		{
			char* const data = reinterpret_cast<char*>(&m_pendingPacket.Size) + m_pendingPacket.SizeReceived;
			const Status status = Receive(data, sizeof(m_pendingPacket.Size) - m_pendingPacket.SizeReceived,
			                              received);
			m_pendingPacket.SizeReceived += received;

			if (status != Status::Done)
				return status;
		}

		//The packet size has been fully received
		packetSize = m_pendingPacket.Size;
		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(packetSize);
	}
	else
	{
		//The packet size has already been received in a previous call
		packetSize = m_pendingPacket.Size;
		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(packetSize);
	}

	//Loop until we receive all the packet data
	std::array<char, 1024> buffer{};
	while(m_pendingPacket.Data.size() < packetSize)
	{
		//Receive a chunk of data
		const std::size_t sizeToGet = std::min(static_cast<std::size_t>(packetSize - m_pendingPacket.Data.size()),
		                                       buffer.size());
		const Status status = Receive(buffer.data(), sizeToGet, received);
		if (status != Status::Done)
			return status;

		//Append it into the packet
		if(received > 0)
		{
			m_pendingPacket.Data.resize(m_pendingPacket.Data.size() + received);
			char* const begin = &m_pendingPacket.Data[0] + m_pendingPacket.Data.size() - received;
			std::copy_n(buffer.data(), received, begin);
		}
	}

	//we have received all the packet data: we can copy it to the user packet
	if (!m_pendingPacket.Data.empty())
		packet.OnReceive(&m_pendingPacket.Data[0], m_pendingPacket.Data.size());

	//Clear the pending packet data
	m_pendingPacket = PendingPacket();

	return Status::Done;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::TCPSocket::PendingPacket::PendingPacket() noexcept
	: Size(0), SizeReceived(0)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
#undef far
#endif