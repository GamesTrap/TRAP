#include "IPv6Tests.h"

IPv6Tests::IPv6Tests()
	: Layer("IPv6")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void IPv6Tests::OnAttach()
{
	m_ipv6Thread = TRAP::MakeScope<std::thread>(IPv6);
	m_ipv6Thread->detach();
}

//-------------------------------------------------------------------------------------------------------------------//

void IPv6Tests::OnDetach()
{
	m_ipv6Thread.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void IPv6Tests::IPv6()
{
	//Print out the local IP
	std::cout << "[EXPERIMENTAL][Network][IPv6] Local Address: " <<
	             TRAP::Network::IPv6Address::GetLocalAddress() << std::endl;
	//Print out the public IP
	std::cout << "[EXPERIMENTAL][Network][IPv6] Public Address: " <<
	             TRAP::Network::IPv6Address::GetPublicAddress() << std::endl;

	//Ask for the server address
	TRAP::Network::IPv6Address server;
	do
	{
		std::cout << "[EXPERIMENTAL][Network][IPv6] Type the address or name of a server: ";
		std::cin >> server;
	} while (server == TRAP::Network::IPv6Address::None);

	//Print out the IP
	std::cout << "[EXPERIMENTAL][Network][IPv6] Entered IP: " << server << std::endl;
}