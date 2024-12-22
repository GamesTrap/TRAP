#include "SocketTests.h"

namespace
{
	constexpr std::string_view NetworkSocketsPrefix = "[Network][Sockets] ";
	constexpr std::string_view NetworkSocketsTCPPrefix = "[Network][Sockets][TCP] ";
	constexpr std::string_view NetworkSocketsUDPPrefix = "[Network][Sockets][UDP] ";
	constexpr std::string_view NetworkSocketsTCPIPv4Prefix = "[Network][Sockets][TCP][IPv4] ";
	constexpr std::string_view NetworkSocketsUDPIPv4Prefix = "[Network][Sockets][UDP][IPv4] ";
	constexpr std::string_view NetworkSocketsTCPIPv6Prefix = "[Network][Sockets][TCP][IPv6] ";
	constexpr std::string_view NetworkSocketsUDPIPv6Prefix = "[Network][Sockets][UDP][IPv6] ";

	void RunTCPServerIPv4(const u16 port)
	{
		//Create a server socket to accept new connections
		TRAP::Network::TCPListener listener;

		//Listen to the given port for incoming connections
		if (listener.Listen(port) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Server is listening to port {}, waiting for connections...", NetworkSocketsTCPIPv4Prefix, port);

		//Wait for a connection
		TRAP::Network::TCPSocket socket;
		if (listener.Accept(socket) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Client connected: {}", NetworkSocketsTCPIPv4Prefix, socket.GetRemoteAddress().Value());

		//Send a message to the connected client
		const std::string out = "Hi, I'm the server";
		if (socket.Send(out.data(), out.size()) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message sent to the client: \"{}\"", NetworkSocketsTCPIPv4Prefix, out);

		//Receive a message back from the client
		std::array<char, 128> in{};
		usize received = 0;
		if (socket.Receive(in.data(), in.size(), received) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Answer received from the client: \"{}\"", NetworkSocketsTCPIPv4Prefix, in.data());
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void RunTCPClientIPv4(const u16 port)
	{
		//Ask for the server address
		TRAP::Optional<TRAP::Network::IPv4Address> server{};
		do
		{
			fmt::print("{}Type the address or name of the server to connect to: ", NetworkSocketsTCPIPv4Prefix);
			std::cin >> server;
		} while (!server);

		//Create a socket for communicating with the server
		TRAP::Network::TCPSocket socket;

		//Connect to the server
		if (socket.Connect(*server, port) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Connected to server ", NetworkSocketsTCPIPv4Prefix, *server);

		//Receive a message from the server
		std::array<char, 128> in{};
		usize received = 0;
		if (socket.Receive(in.data(), in.size(), received) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message received from the server: \"{}\"", NetworkSocketsTCPIPv4Prefix, in.data());

		//Send an answer to the server
		const std::string out = "Hi, I'm a client";
		if (socket.Send(out.data(), out.size()) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message sent to the server: \"{}\"", NetworkSocketsTCPIPv4Prefix, out);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void RunUDPServerIPv4(const u16 port)
	{
		//Create a socket to receive a message from anyone
		TRAP::Network::UDPSocket socket;

		//Listen to messages on the specified port
		if (socket.Bind(port) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Server is listening to port {}, waiting for a message...", NetworkSocketsUDPIPv4Prefix, port);

		//Wait for a message
		std::array<char, 128> in{};
		usize received = 0;
		TRAP::Optional<TRAP::Network::IPv4Address> sender{};
		u16 senderPort = 0u;
		if (socket.Receive(in.data(), in.size(), received, sender, senderPort) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message received from client {}: \"{}\"", NetworkSocketsUDPIPv4Prefix, *sender, in.data());

		//Send an answer to the client
		const std::string out = "Hi, I'm the server";
		if (socket.Send(out.data(), out.size(), *sender, senderPort) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message sent to the client: \"{}\"", NetworkSocketsUDPIPv4Prefix, out);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void RunUDPClientIPv4(const u16 port)
	{
		//Ask for the server address
		TRAP::Optional<TRAP::Network::IPv4Address> server{};
		do
		{
			fmt::print("{}Type the address or name of the server to connect to: ", NetworkSocketsUDPIPv4Prefix);
			std::cin >> server;
		} while (!server);

		//Create a socket for communicating with the server
		TRAP::Network::UDPSocket socket;

		//Send a message to the server
		const std::string out = "Hi, I'm a client";
		if (socket.Send(out.data(), out.size(), *server, port) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message sent to the server: \"{}\"", NetworkSocketsUDPIPv4Prefix, out);

		//Receive an answer from anyone (but most likely from the server)
		std::array<char, 128> in{};
		usize received = 0;
		TRAP::Optional<TRAP::Network::IPv4Address> sender{};
		u16 senderPort = 0;
		if (socket.Receive(in.data(), in.size(), received, sender, senderPort) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message received from {}: \"{}\"", NetworkSocketsUDPIPv4Prefix, *sender, in.data());
	}

	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	void RunTCPServerIPv6(const u16 port)
	{
		//Create a server socket to accept new connections
		TRAP::Network::TCPListenerIPv6 listener;

		//Listen to the given port for incoming connections
		if (listener.Listen(port) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Server is listening to port {}, waiting for connections...", NetworkSocketsTCPIPv6Prefix, port);

		//Wait for a connection
		TRAP::Network::TCPSocketIPv6 socket;
		if (listener.Accept(socket) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Client connected: {}", NetworkSocketsTCPIPv6Prefix, socket.GetRemoteAddress().Value());

		//Send a message to the connected client
		const std::string out = "Hi, I'm the server";
		if (socket.Send(out.data(), out.size()) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message sent to the client: \"{}\"", NetworkSocketsTCPIPv6Prefix, out);

		//Receive a message back from the client
		std::array<char, 128> in{};
		usize received = 0;
		if (socket.Receive(in.data(), in.size(), received) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Answer received from the client: \"{}\"", NetworkSocketsTCPIPv6Prefix, in.data());
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void RunTCPClientIPv6(const u16 port)
	{
		//Ask for the server address
		TRAP::Optional<TRAP::Network::IPv6Address> server{};
		do
		{
			fmt::print("{}Type the address or name of the server to connect to: ", NetworkSocketsTCPIPv6Prefix);
			std::cin >> server;
		} while (!server);

		//Create a socket for communicating with the server
		TRAP::Network::TCPSocketIPv6 socket;

		//Connect to the server
		if (socket.Connect(*server, port) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Connected to server {}", NetworkSocketsTCPIPv6Prefix, *server);

		//Receive a message from the server
		std::array<char, 128> in{};
		usize received = 0;
		if (socket.Receive(in.data(), in.size(), received) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message received from the server: \"{}\"", NetworkSocketsTCPIPv6Prefix, in.data());

		//Send an answer to the server
		const std::string out = "Hi, I'm the client";
		if (socket.Send(out.data(), out.size()) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message sent to the server: \"{}\"", NetworkSocketsTCPIPv6Prefix, out);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void RunUDPServerIPv6(const u16 port)
	{
		//Create a socket to receive a message from anyone
		TRAP::Network::UDPSocketIPv6 socket;

		//Listen to messages on the specified port
		if (socket.Bind(port) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Server is listening to port {}, waiting for a message...", NetworkSocketsUDPIPv6Prefix, port);

		//Wait for a message
		std::array<char, 128> in{};
		usize received = 0;
		TRAP::Optional<TRAP::Network::IPv6Address> sender{};
		u16 senderPort = 0;
		if (socket.Receive(in.data(), in.size(), received, sender, senderPort) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message received from client {}: \"{}\"", NetworkSocketsUDPIPv6Prefix, *sender, in.data());

		//Send an answer to the client
		const std::string out = "Hi, I'm a server";
		if (socket.Send(out.data(), out.size(), *sender, senderPort) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message sent to the client: \"{}\"", NetworkSocketsUDPIPv6Prefix, out);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void RunUDPClientIPv6(const u16 port)
	{
		//Ask for the server address
		TRAP::Optional<TRAP::Network::IPv6Address> server{};
		do
		{
			fmt::print("{}Type the address or name of the server to connect to: ", NetworkSocketsUDPIPv6Prefix);
			std::cin >> server;
		} while (!server);

		//Create a socket for communicating with the server
		TRAP::Network::UDPSocketIPv6 socket;

		//Send a message to the server
		const std::string out = "Hi, I'm a client";
		if (socket.Send(out.data(), out.size(), *server, port) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message sent to the server: \"{}\"", NetworkSocketsUDPIPv6Prefix, out);

		//Receive an answer from anyone (but most likely from the server)
		std::array<char, 128> in{};
		usize received = 0;
		TRAP::Optional<TRAP::Network::IPv6Address> sender{};
		u16 senderPort = 0;
		if (socket.Receive(in.data(), in.size(), received, sender, senderPort) != TRAP::Network::Socket::Status::Done)
			return;
		fmt::println("{}Message received from {}: \"{}\"", NetworkSocketsUDPIPv6Prefix, *sender, in.data());
	}

	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	void Sockets()
	{
		//Choose an arbitrary port for opening sockets
		static constexpr u16 port = 50001;

		//IPv4 or IPv6?
		char IPVersion = '4';
		fmt::print("{}Do you want to use IPv6(6) or IPv4(4)? ", NetworkSocketsPrefix);
		std::cin >> IPVersion;

		//TCP, UDP or connected UDP?
		char protocol = 't';
		fmt::print("{}Do you want to use TCP(t) or UDP(u)? ", NetworkSocketsPrefix);
		std::cin >> protocol;

		//Client or server?
		if (protocol == 't')
			fmt::print("{}Do you want to be a server(s) or a client(c)? ", NetworkSocketsTCPPrefix);
		else
			fmt::print("{}Do you want to be a server(s) or a client(c)? ", NetworkSocketsUDPPrefix);
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
}

//-------------------------------------------------------------------------------------------------------------------//

void SocketTests::OnAttach()
{
	std::thread socketThread(Sockets);
	socketThread.detach();
}
