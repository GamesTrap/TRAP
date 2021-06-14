/*
////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2019 Laurent Gomila (laurent@sfml-dev.org)
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

#include "Network/Packet.h"
#include "Network/IP/IPv4Address.h"
#include "SocketImpl.h"

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

TRAP::Network::TCPSocket::TCPSocket()
	: Socket(Type::TCP)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint16_t TRAP::Network::TCPSocket::GetLocalPort() const
{
	if(GetHandle() != INTERNAL::Network::SocketImpl::InvalidSocket())
	{
		//Retrieve information about the local end of the socket
		sockaddr_in address{};
		INTERNAL::Network::SocketImpl::AddressLength size = sizeof(address);
		if (getsockname(GetHandle(), reinterpret_cast<sockaddr*>(&address), &size) != -1)
			return ntohs(address.sin_port);
	}

	//We failed to retrieve the port
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::IPv4Address TRAP::Network::TCPSocket::GetRemoteAddress() const
{
	if(GetHandle() != INTERNAL::Network::SocketImpl::InvalidSocket())
	{
		//Retrieve information about the remote end of the socket
		sockaddr_in address{};
		INTERNAL::Network::SocketImpl::AddressLength size = sizeof(address);
		if (getpeername(GetHandle(), reinterpret_cast<sockaddr*>(&address), &size) != -1)
			return IPv4Address(ntohl(address.sin_addr.s_addr));
	}

	//We failed to retrieve the address
	return IPv4Address::None;
}

//-------------------------------------------------------------------------------------------------------------------//

uint16_t TRAP::Network::TCPSocket::GetRemotePort() const
{
	if(GetHandle() != INTERNAL::Network::SocketImpl::InvalidSocket())
	{
		//Retrieve information about the remote end of the socket
		sockaddr_in address{};
		INTERNAL::Network::SocketImpl::AddressLength size = sizeof(address);
		if (getpeername(GetHandle(), reinterpret_cast<sockaddr*>(&address), &size) != -1)
			return ntohs(address.sin_port);
	}

	//We failed to retrieve the port
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::TCPSocket::Connect(const IPv4Address& remoteAddress, const uint16_t remotePort, Utils::TimeStep timeout)
{
	//Disconnect the socket if it is already connected
	Disconnect();

	//Create the internal socket if it doesn't exist
	CreateIPv4();

	//Create the remote address
	sockaddr_in address = INTERNAL::Network::SocketImpl::CreateAddress(remoteAddress.ToInteger(), remotePort);

	if(timeout <= Utils::TimeStep(0.0f))
	{
		//We're not using a timeout: just try to connect

		//Connect the socket
		if (::connect(GetHandle(), reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1)
			return INTERNAL::Network::SocketImpl::GetErrorStatus();

		//Connection succeeded
		return Status::Done;
	}
	else
	{
		//We're using a timeout: we'll need a few tricks to make it work

		//save the previous blocking state
		const bool blocking = IsBlocking();

		//Switch to non-blocking to enable our connection timeout
		if (blocking)
			SetBlocking(false);

		//Try to connect to the remote address
		if(::connect(GetHandle(), reinterpret_cast<sockaddr*>(&address), sizeof(address)) >= 0)
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
			timeval time;
			time.tv_sec = static_cast<long>(timeout.GetSeconds());
			time.tv_usec = static_cast<long>(timeout.GetSeconds());

			//Wait for something to write on our socket (which means that the connection request has returned)
			if(select(static_cast<int>(GetHandle() + 1), nullptr, &selector, nullptr, &time) > 0)
			{
				//At this point the connection may have been either accepted or refused.
				//To know whether it's a success or a failure, we must check the address of the connected peer
				if(GetRemoteAddress() != IPv4Address::None)
				{
					//Connection accepted
					status = Status::Done;
				}
				else
				{
					//Conncetion refused
					status = INTERNAL::Network::SocketImpl::GetErrorStatus();
				}
			}
			else
			{
				//Failed to connect before timeout is over
				status = INTERNAL::Network::SocketImpl::GetErrorStatus();
			}
		}

		//Switch back to blocking mode
		SetBlocking(true);

		return status;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::TCPSocket::Disconnect()
{
	//Close the socket
	Close();

	//Reset the pending packet data
	m_pendingPacket = PendingPacket();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::TCPSocket::Send(const void* data, const std::size_t size) const
{
	if (!IsBlocking())
		TP_WARN(Log::NetworkTCPSocketPrefix, "Partial sends might not be handled properly.");

	std::size_t sent;

	return Send(data, size, sent);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::TCPSocket::Send(const void* data, const std::size_t size, std::size_t& sent) const
{
	//Check the parameters
	if(!data || (size == 0))
	{
		TP_ERROR(Log::NetworkTCPSocketPrefix, "Cannot send data over the network (no data to send)");
		return Status::Error;
	}

	//Loop until every byte has been sent
	int32_t result;
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

TRAP::Network::Socket::Status TRAP::Network::TCPSocket::Receive(void* data, const std::size_t size, std::size_t& received) const
{
	//First clear the variables to fill
	received = 0;

	//Check the destination buffer
	if(!data)
	{
		TP_ERROR(Log::NetworkTCPSocketPrefix, "Cannot receive data from the network (the destination buffer is invalid)");
		return Status::Error;
	}

	//Receive a chunk of bytes
	const int32_t sizeReceived = recv(GetHandle(), static_cast<char*>(data), static_cast<int>(size), flags);

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
	//TCP is a stream protocol, it doesn't preserve messages boundaries.
	//This means that we have to send the packet size first, so that the
	//receiver knows the actual end of the packet in the data stream.

	//We allocate an extra memory block so that the size can be sent
	//together with the data in a single call.
	//This may seem inefficient, but is is actually required to avoid
	//partial send, which could cause data corruption on the receiving end.

	//Get the data to send from the packet
	std::size_t size = 0;
	const void* data = packet.OnSend(size);

	//First convert the packet size to network byte order
	uint32_t packetSize = htonl(static_cast<uint32_t>(size));

	//Allocate memory for the data block to send
	std::vector<char> blockToSend(sizeof(packetSize) + size);

	//Copy the packet size and data into the block to send
	std::memcpy(&blockToSend[0], &packetSize, sizeof(packetSize));
	if (size > 0)
		std::memcpy(&blockToSend[0] + sizeof(packetSize), data, size);

	//Send the data block
	std::size_t sent;
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
	//First clear the variables to fill
	packet.Clear();

	//We start by getting the size of the incoming packet
	uint32_t packetSize;
	std::size_t received = 0;
	if(m_pendingPacket.SizeReceived < sizeof(m_pendingPacket.Size))
	{
		//Loop until we've received the entire size of the packet
		//(event a 4 byte variable may be received in more than one call)
		while(m_pendingPacket.SizeReceived < sizeof(m_pendingPacket.Size))
		{
			char* data = reinterpret_cast<char*>(&m_pendingPacket.Size) + m_pendingPacket.SizeReceived;
			const Status status = Receive(data, sizeof(m_pendingPacket.Size) - m_pendingPacket.SizeReceived, received);
			m_pendingPacket.SizeReceived += received;

			if (status != Status::Done)
				return status;
		}

		//The packet size has been fully received
		packetSize = ntohl(m_pendingPacket.Size);
	}
	else
	{
		//The packet size has already been received in a previous call
		packetSize = ntohl(m_pendingPacket.Size);
	}

	//Loop until we receive all the packet data
	std::array<char, 1024> buffer{};
	while(m_pendingPacket.Data.size() < packetSize)
	{
		//Receive a chunk of data
		const std::size_t sizeToGet = std::min(static_cast<std::size_t>(packetSize - m_pendingPacket.Data.size()), buffer.size());
		const Status status = Receive(buffer.data(), sizeToGet, received);
		if (status != Status::Done)
			return status;

		//Append it into the packet
		if(received > 0)
		{
			m_pendingPacket.Data.resize(m_pendingPacket.Data.size() + received);
			char* begin = &m_pendingPacket.Data[0] + m_pendingPacket.Data.size() - received;
			std::memcpy(begin, buffer.data(), received);
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

TRAP::Network::TCPSocket::PendingPacket::PendingPacket()
	: Size(0), SizeReceived(0)
{
}