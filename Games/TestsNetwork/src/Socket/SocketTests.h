#ifndef _GAMESTRAP_SOCKETTESTS_H_
#define _GAMESTRAP_SOCKETTESTS_H_

#include <TRAP.h>

class SocketTests final : public TRAP::Layer
{
public:
	SocketTests();

	void OnAttach() override;
	void OnDetach() override;

private:
	static void Sockets();

	static void RunTCPServerIPv4(uint16_t port);
	static void RunTCPClientIPv4(uint16_t port);
	static void RunUDPServerIPv4(uint16_t port);
	static void RunUDPClientIPv4(uint16_t port);

	static void RunTCPServerIPv6(uint16_t port);
	static void RunTCPClientIPv6(uint16_t port);
	static void RunUDPServerIPv6(uint16_t port);
	static void RunUDPClientIPv6(uint16_t port);

	TRAP::Scope<std::thread> m_socketThread;
};

#endif /*_GAMESTRAP_SOCKETTESTS_H_*/