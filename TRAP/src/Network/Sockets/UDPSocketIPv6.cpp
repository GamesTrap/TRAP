#include "TRAPPCH.h"
#include "UDPSocketIPv6.h"

#include "Core/PlatformDetection.h"
#include "Network/Packet.h"
#include "SocketImpl.h"
#include "Utils/Utils.h"
#include "Utils/ByteSwap.h"

TRAP::Network::UDPSocketIPv6::UDPSocketIPv6()
	: Socket(Type::UDP), m_buffer(MaxDatagramSize)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint16_t TRAP::Network::UDPSocketIPv6::GetLocalPort() const
{
	if(GetHandle() == INTERNAL::Network::SocketImpl::InvalidSocket())
		return 0; //We failed to retrieve the port

	//Retrieve information about the local end of the socket
	sockaddr_in6 address{};
	INTERNAL::Network::SocketImpl::AddressLength size = sizeof(address);
	if (getsockname(GetHandle(), reinterpret_cast<sockaddr*>(&address), &size) != -1)
	{
		uint16_t port = address.sin6_port;

		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(port);

		return port;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::UDPSocketIPv6::Bind(const uint16_t port, const IPv6Address& address)
{
	//Close the socket if it is already bound
	Close();

	//Create the internal socket if it doesn't exist
	CreateIPv6();

	//Check if the address is valid
	if ((address == IPv6Address::None))
		return Status::Error;

	//Bind the socket
	sockaddr_in6 addr = INTERNAL::Network::SocketImpl::CreateAddress(address.ToArray(), port);
	if(::bind(GetHandle(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
	{
		TP_ERROR(Log::NetworkUDPSocketPrefix, "Failed to bind socket to port");
		return Status::Error;
	}

	return Status::Done;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::UDPSocketIPv6::Unbind()
{
	//Simply close the socket
	Close();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::UDPSocketIPv6::Send(const void* data, const std::size_t size,
                                                                 const IPv6Address& remoteAddress,
																 uint16_t remotePort)
{
	//Create the internal socket if it doesn't exist
	CreateIPv6();

	//Make sure that all the data will fit in one datagram
	if(size > MaxDatagramSize)
	{
		TP_ERROR(Log::NetworkUDPSocketPrefix, "Can't send data over the network (the number of bytes",
			" to send is greater than TRAP::Network::UDPSocket::MaxDatagramSize)");
		return Status::Error;
	}

	//Build the target address
	sockaddr_in6 address = INTERNAL::Network::SocketImpl::CreateAddress(remoteAddress.ToArray(), remotePort);

	//Send the data (unlike TCP, all the data is always sent in one call)
	const int32_t sent = sendto(GetHandle(), static_cast<const char*>(data), static_cast<int32_t>(size), 0,
	                            reinterpret_cast<sockaddr*>(&address), sizeof(address));

	//Check for errors
	if (sent < 0)
		return INTERNAL::Network::SocketImpl::GetErrorStatus();

	return Status::Done;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::UDPSocketIPv6::Receive(void* data, const std::size_t size,
                                                                    std::size_t& received,
																	IPv6Address& remoteAddress,
																	uint16_t& remotePort) const
{
	//First clear the variables to fill
	received = 0;
	remoteAddress = IPv6Address();
	remotePort = 0;

	//Check the destination buffer
	if(!data)
	{
		TP_ERROR(Log::NetworkUDPSocketPrefix,
		         "Can't receive data from the network (the destination buffer is invalid)");
		return Status::Error;
	}

	//Data that will be filled with the other computer's address
	std::array<uint8_t, 16> addr{};
#ifdef TRAP_PLATFORM_WINDOWS
	std::memcpy(addr.data(), in6addr_any.u.Byte, addr.size());
#else
	std::memcpy(addr.data(), in6addr_any.s6_addr, addr.size());
#endif
	sockaddr_in6 address = INTERNAL::Network::SocketImpl::CreateAddress(addr, 0);

	//Receive a chunk of bytes
	INTERNAL::Network::SocketImpl::AddressLength addressSize = sizeof(address);
	const int32_t sizeReceived = recvfrom(GetHandle(), static_cast<char*>(data), static_cast<int32_t>(size), 0,
	                                      reinterpret_cast<sockaddr*>(&address), &addressSize);

	//Check for errors
	if (sizeReceived < 0)
		return INTERNAL::Network::SocketImpl::GetErrorStatus();

	//Fill the sender information
	received = static_cast<std::size_t>(sizeReceived);
	addr = {};
#ifdef TRAP_PLATFORM_WINDOWS
	std::memcpy(addr.data(), address.sin6_addr.u.Byte, addr.size());
#else
	std::memcpy(addr.data(), address.sin6_addr.s6_addr, addr.size());
#endif
	remoteAddress = IPv6Address(addr);

	uint16_t port = address.sin6_port;

	if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(port);

	remotePort = port;

	return Status::Done;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::UDPSocketIPv6::Send(Packet& packet, const IPv6Address& remoteAddress,
                                                                 const uint16_t remotePort)
{
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
	const void* data = packet.OnSend(size);

	//Send it
	return Send(data, size, remoteAddress, remotePort);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Socket::Status TRAP::Network::UDPSocketIPv6::Receive(Packet& packet, IPv6Address& remoteAddress,
                                                                    uint16_t& remotePort)
{
	//See the detailed comment in Send(Packet) above.

	//Receive the datagram
	std::size_t received = 0;
	const Status status = Receive(&m_buffer[0], m_buffer.size(), received, remoteAddress, remotePort);

	//If we received valid data, we can copy it to the user packet
	packet.Clear();
	if ((status == Status::Done) && (received > 0))
		packet.OnReceive(&m_buffer[0], received);

	return status;
}