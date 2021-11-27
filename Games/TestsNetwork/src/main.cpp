#include <TRAP.h>

#include "FTP/FTPTests.h"
#include "Socket/SocketTests.h"
#include "IPv6/IPv6Tests.h"

class TestsNetwork final : public TRAP::Application
{
public:
	explicit TestsNetwork(std::string gameName)
		: Application(std::move(gameName))
	{
		PushLayer(TRAP::MakeScope<FTPTests>());
		//PushLayer(TRAP::MakeScope<SocketTests>());
		//PushLayer(TRAP::MakeScope<IPv6Tests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<TestsNetwork>("TestsNetwork");
}