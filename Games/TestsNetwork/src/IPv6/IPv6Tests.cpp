#include "IPv6Tests.h"

namespace
{
	constexpr std::string_view NetworkIPv6Prefix = "[Network][IPv6] ";

	void IPv6()
	{
		//Print out the local IP
		fmt::println("{}Local Address: {}", NetworkIPv6Prefix, TRAP::Network::IPv6Address::GetLocalAddress());
		//Print out the public IP
		fmt::println("{}Public Address: {}", NetworkIPv6Prefix, TRAP::Network::IPv6Address::GetPublicAddress());

		//Ask for the server address
		TRAP::Network::IPv6Address server{};
		do
		{
			fmt::print("{}Type the address or name of a server: ", NetworkIPv6Prefix);
			std::cin >> server;
		} while (server == TRAP::Network::IPv6Address::None);

		//Print out the IP
		fmt::println("{}Entered IP: ", NetworkIPv6Prefix, server);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void IPv6Tests::OnAttach()
{
	std::thread ipv6Thread(IPv6);
	ipv6Thread.detach();
}
