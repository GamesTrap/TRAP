#ifndef GAMESTRAP_SOCKETTESTS_H
#define GAMESTRAP_SOCKETTESTS_H

#include <TRAP.h>

class SocketTests final : public TRAP::Layer
{
public:
	constexpr SocketTests();

	void OnAttach() override;

private:
	static void Sockets();

	static void RunTCPServerIPv4(u16 port);
	static void RunTCPClientIPv4(u16 port);
	static void RunUDPServerIPv4(u16 port);
	static void RunUDPClientIPv4(u16 port);

	static void RunTCPServerIPv6(u16 port);
	static void RunTCPClientIPv6(u16 port);
	static void RunUDPServerIPv6(u16 port);
	static void RunUDPClientIPv6(u16 port);

	std::unique_ptr<std::thread> m_socketThread = nullptr;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr SocketTests::SocketTests()
	: Layer("SocketTests")
{
}

#endif /*GAMESTRAP_SOCKETTESTS_H*/