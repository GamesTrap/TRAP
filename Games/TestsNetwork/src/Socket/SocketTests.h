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
	
	static void RunTCPServer(uint16_t port);
	static void RunTCPClient(uint16_t port);
	static void RunUDPServer(uint16_t port);
	static void RunUDPClient(uint16_t port);
	
	TRAP::Scope<std::thread> m_socketThread;
};

#endif /*_GAMESTRAP_SOCKETTESTS_H_*/