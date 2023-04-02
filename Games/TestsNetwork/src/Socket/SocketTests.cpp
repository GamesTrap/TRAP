#include "SocketTests.h"

SocketTests::SocketTests()
	: Layer("SocketTests")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::OnAttach()
{
	m_socketThread = std::make_unique<std::thread>(Sockets);
	if(m_socketThread)
		m_socketThread->detach();
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::OnDetach()
{
	m_socketThread.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::Sockets()
{
	//Choose an arbitrary port for opening sockets
	static constexpr uint16_t port = 50001;

	//IPv4 or IPv6?
	char IPVersion = '4';
	std::cout << "[Network][Sockets] Do you want to use IPv6(6) or IPv4(4)? ";
	std::cin >> IPVersion;

	//TCP, UDP or connected UDP?
	char protocol = 't';
	std::cout << "[Network][Sockets] Do you want to use TCP(t) or UDP(u)? ";
	std::cin >> protocol;

	//Client or server?
	if (protocol == 't')
		std::cout << "[Network][Sockets][TCP] Do you want to be a server(s) or a client(c)? ";
	else
		std::cout << "[Network][Sockets][UDP] Do you want to be a server(s) or a client(c)? ";
	char who = 's';
	std::cin >> who;

	if(IPVersion == '6')
	{
		if (protocol == 't')
		{
			//Test the TCP protocol
			if (who == 's')
				RunTCPServerIPv6(port);
			else
				RunTCPClientIPv6(port);
		}
		else
		{
			//Test the unconnected UDP protocol
			if (who == 's')
				RunUDPServerIPv6(port);
			else
				RunUDPClientIPv6(port);
		}
	}
	else
	{
		if (protocol == 't')
		{
			//Test the TCP protocol
			if (who == 's')
				RunTCPServerIPv4(port);
			else
				RunTCPClientIPv4(port);
		}
		else
		{
			//Test the unconnected UDP protocol
			if (who == 's')
				RunUDPServerIPv4(port);
			else
				RunUDPClientIPv4(port);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunTCPServerIPv4(const uint16_t port)
{
	//Create a server socket to accept new connections
	TRAP::Network::TCPListener listener;

	//Listen to the given port for incoming connections
	if (listener.Listen(port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv4] Server is listening to port " << port <<
	             ", waiting for connections... " << std::endl;

	//Wait for a connection
	TRAP::Network::TCPSocket socket;
	if (listener.Accept(socket) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv4] Client connected: " << socket.GetRemoteAddress() << std::endl;

	//Send a message to the connected client
	const std::string out = "Hi, I'm the server";
	if (socket.Send(out.data(), out.size()) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv4] Message sent to the client: \"" << out << "\"" << std::endl;

	//Receive a message back from the client
	std::array<char, 128> in{};
	std::size_t received = 0;
	if (socket.Receive(in.data(), in.size(), received) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv4] Answer received from the client: \"" << in.data() << "\"" <<
	             std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunTCPClientIPv4(const uint16_t port)
{
	//Ask for the server address
	TRAP::Network::IPv4Address server{};
	do
	{
		std::cout << "[Network][Sockets][TCP][IPv4] Type the address or name of the server to connect to: ";
		std::cin >> server;
	} while (server == TRAP::Network::IPv4Address::None);

	//Create a socket for communicating with the server
	TRAP::Network::TCPSocket socket;

	//Connect to the server
	if (socket.Connect(server, port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv4] Connected to server " << server << std::endl;

	//Receive a message from the server
	std::array<char, 128> in{};
	std::size_t received = 0;
	if (socket.Receive(in.data(), in.size(), received) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv4] Message received from the server: \"" << in.data() << "\"" <<
	             std::endl;

	//Send an answer to the server
	const std::string out = "Hi, I'm a client";
	if (socket.Send(out.data(), out.size()) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv4] Message sent to the server: \"" << out << "\"" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunUDPServerIPv4(const uint16_t port)
{
	//Create a socket to receive a message from anyone
	TRAP::Network::UDPSocket socket;

	//Listen to messages on the specified port
	if (socket.Bind(port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP][IPv4] Server is listening to port " << port <<
	             ", waiting for a message... " << std::endl;

	//Wait for a message
	std::array<char, 128> in{};
	std::size_t received = 0;
	TRAP::Network::IPv4Address sender{};
	uint16_t senderPort = 0;
	if (socket.Receive(in.data(), in.size(), received, sender, senderPort) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP][IPv4] Message received from client " << sender << ": \"" <<
	             in.data() << "\"" << std::endl;

	//Send an answer to the client
	const std::string out = "Hi, I'm the server";
	if (socket.Send(out.data(), out.size(), sender, senderPort) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP][IPv4] Message sent to the client: \"" << out << "\"" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunUDPClientIPv4(const uint16_t port)
{
	//Ask for the server address
	TRAP::Network::IPv4Address server{};
	do
	{
		std::cout << "[Network][Sockets][UDP][IPv4] Type the address or name of the server to connect to: ";
		std::cin >> server;
	} while (server == TRAP::Network::IPv4Address::None);

	//Create a socket for communicating with the server
	TRAP::Network::UDPSocket socket;

	//Send a message to the server
	const std::string out = "Hi, I'm a client";
	if (socket.Send(out.data(), out.size(), server, port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP][IPv4] Message sent to the server: \"" << out << "\"" << std::endl;

	//Receive an answer from anyone (but most likely from the server)
	std::array<char, 128> in{};
	std::size_t received = 0;
	TRAP::Network::IPv4Address sender{};
	uint16_t senderPort = 0;
	if (socket.Receive(in.data(), in.size(), received, sender, senderPort) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP][IPv4] Message received from " << sender << ": \"" << in.data() <<
	             "\"" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunTCPServerIPv6(const uint16_t port)
{
	//Create a server socket to accept new connections
	TRAP::Network::TCPListenerIPv6 listener;

	//Listen to the given port for incoming connections
	if (listener.Listen(port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv6] Server is listening to port " << port <<
	             ", waiting for connections... " << std::endl;

	//Wait for a connection
	TRAP::Network::TCPSocketIPv6 socket;
	if (listener.Accept(socket) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv6] Client connected: " << socket.GetRemoteAddress() << std::endl;

	//Send a message to the connected client
	const std::string out = "Hi, I'm the server";
	if (socket.Send(out.data(), out.size()) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv6] Message sent to the client: \"" << out << "\"" << std::endl;

	//Receive a message back from the client
	std::array<char, 128> in{};
	std::size_t received = 0;
	if (socket.Receive(in.data(), in.size(), received) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv6] Answer received from the client: \"" << in.data() <<
	             "\"" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunTCPClientIPv6(const uint16_t port)
{
	//Ask for the server address
	TRAP::Network::IPv6Address server{};
	do
	{
		std::cout << "[Network][Sockets][TCP][IPv6] Type the address or name of the server to connect to: ";
		std::cin >> server;
	} while (server == TRAP::Network::IPv6Address::None);

	//Create a socket for communicating with the server
	TRAP::Network::TCPSocketIPv6 socket;

	//Connect to the server
	if (socket.Connect(server, port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv6] Connected to server " << server << std::endl;

	//Receive a message from the server
	std::array<char, 128> in{};
	std::size_t received = 0;
	if (socket.Receive(in.data(), in.size(), received) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv6] Message received from the server: \"" << in.data() <<
	             "\"" << std::endl;

	//Send an answer to the server
	const std::string out = "Hi, I'm the client";
	if (socket.Send(out.data(), out.size()) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP][IPv6] Message sent to the server: \"" << out << "\"" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunUDPServerIPv6(const uint16_t port)
{
	//Create a socket to receive a message from anyone
	TRAP::Network::UDPSocketIPv6 socket;

	//Listen to messages on the specified port
	if (socket.Bind(port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP][IPv6] Server is listening to port " << port <<
	             ", waiting for a message... " << std::endl;

	//Wait for a message
	std::array<char, 128> in{};
	std::size_t received = 0;
	TRAP::Network::IPv6Address sender{};
	uint16_t senderPort = 0;
	if (socket.Receive(in.data(), in.size(), received, sender, senderPort) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP][IPv6] Message received from client " << sender << ": \"" <<
	             in.data() << "\"" << std::endl;

	//Send an answer to the client
	const std::string out = "Hi, I'm a server";
	if (socket.Send(out.data(), out.size(), sender, senderPort) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP][IPv6] Message sent to the client: \"" << out << "\"" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunUDPClientIPv6(const uint16_t port)
{
	//Ask for the server address
	TRAP::Network::IPv6Address server{};
	do
	{
		std::cout << "[Network][Sockets][UDP][IPv6] Type the address or name of the server to connect to: ";
		std::cin >> server;
	} while (server == TRAP::Network::IPv6Address::None);

	//Create a socket for communicating with the server
	TRAP::Network::UDPSocketIPv6 socket;

	//Send a message to the server
	const std::string out = "Hi, I'm a client";
	if (socket.Send(out.data(), out.size(), server, port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP][IPv6] Message sent to the server: \"" << out << "\"" << std::endl;

	//Receive an answer from anyone (but most likely from the server)
	std::array<char, 128> in{};
	std::size_t received = 0;
	TRAP::Network::IPv6Address sender{};
	uint16_t senderPort = 0;
	if (socket.Receive(in.data(), in.size(), received, sender, senderPort) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP][IPv6] Message received from " << sender << ": \"" << in.data() <<
	             "\"" << std::endl;
}