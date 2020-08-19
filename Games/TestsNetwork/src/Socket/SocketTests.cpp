#include "SocketTests.h"

SocketTests::SocketTests()
	: Layer("SocketTests")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::OnAttach()
{
	m_socketThread = TRAP::MakeScope<std::thread>(Sockets);
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
	constexpr uint16_t port = 50001;

	//TCP, UDP or connected UDP?
	char protocol;
	std::cout << "[Network][Sockets] Do you want to use TCP(t) or UDP(u)? ";
	std::cin >> protocol;

	//Client or server?
	char who;
	if (protocol == 't')
		std::cout << "[Network][Sockets][TCP] Do you want to be a server(s) or a client(c)? ";
	else
		std::cout << "[Network][Sockets][UDP] Do you want to be a server(s) or a client(c)? ";
	std::cin >> who;

	if(protocol == 't')
	{
		//Test the TCP protocol
		if (who == 's')
			RunTCPServer(port);
		else
			RunTCPClient(port);
	}
	else
	{
		//Test the unconnected UDP protocol
		if (who == 's')
			RunUDPServer(port);
		else
			RunUDPClient(port);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunTCPServer(const uint16_t port)
{
	//Create a server socket to accept new connections
	TRAP::Network::TCPListener listener;

	//Listen to the given port for incoming connections
	if (listener.Listen(port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP] Server is listening to port " << port << ", waiting for connections... " << std::endl;

	//Wait for a connection
	TRAP::Network::TCPSocket socket;
	if (listener.Accept(socket) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP] Client connected: " << socket.GetRemoteAddress() << std::endl;

	//Send a message to the connected client
	const char out[] = "Hi, I'm the server";
	if (socket.Send(out, sizeof(out)) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP] Message sent to the client: \"" << out << "\"" << std::endl;

	//Receive a message back from the client
	std::array<char, 128> in{};
	std::size_t received;
	if (socket.Receive(in.data(), in.size(), received) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP] Answer received from the client: \"" << in.data() << "\"" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunTCPClient(const uint16_t port)
{
	//Ask for the server address
	TRAP::Network::IPv4Address server;
	do
	{
		std::cout << "[Network][Sockets][TCP] Type the address or name of the server to connect to: ";
		std::cin >> server;
	} while (server == TRAP::Network::IPv4Address::None);

	//Create a socket for communicating with the server
	TRAP::Network::TCPSocket socket;

	//Connect to the server
	if (socket.Connect(server, port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP] Connected to server " << server << std::endl;

	//Receive a message from the server
	std::array<char, 128> in{};
	std::size_t received;
	if (socket.Receive(in.data(), in.size(), received) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP] Message received from the server: \"" << in.data() << "\"" << std::endl;

		//Send an answer to the server
	const char out[] = "Hi, I'm a client";
	if (socket.Send(out, sizeof(out)) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][TCP] Message sent to the server: \"" << out << "\"" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunUDPServer(const uint16_t port)
{
	//Create a socket to receive a message from anyone
	TRAP::Network::UDPSocket socket;

	//Listen to messages on the specified port
	if (socket.Bind(port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP] Server is listening to port " << port << ", waiting for a message... " << std::endl;

	//Wait for a message
	std::array<char, 128> in{};
	std::size_t received;
	TRAP::Network::IPv4Address sender;
	uint16_t senderPort;
	if (socket.Receive(in.data(), in.size(), received, sender, senderPort) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP] Message received from client " << sender << ": \"" << in.data() << "\"" << std::endl;

	//Send an answer to the client
	const char out[] = "Hi, I'm the server";
	if (socket.Send(out, sizeof(out), sender, senderPort) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP] Message sent to the client: \"" << out << "\"" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::RunUDPClient(uint16_t port)
{
	//Ask for the server address
	TRAP::Network::IPv4Address server;
	do
	{
		std::cout << "[Network][Sockets][UDP] Type the address or name of the server to connect to: ";
		std::cin >> server;
	} while (server == TRAP::Network::IPv4Address::None);

	//Create a socket for communicating with the server
	TRAP::Network::UDPSocket socket;

	//Send a message to the server
	const char out[] = "Hi, I'm a client";
	if (socket.Send(out, sizeof(out), server, port) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP] Message sent to the server: \"" << out << "\"" << std::endl;

	//Receive an answer from anyone (but most likely from the server)
	std::array<char, 128> in{};
	std::size_t received;
	TRAP::Network::IPv4Address sender;
	uint16_t senderPort;
	if (socket.Receive(in.data(), in.size(), received, sender, senderPort) != TRAP::Network::Socket::Status::Done)
		return;
	std::cout << "[Network][Sockets][UDP] Message received from " << sender << ": \"" << in.data() << "\"" << std::endl;
}