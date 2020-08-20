#include <TRAP.h>

#include "FTP/FTPTests.h"
#include "Socket/SocketTests.h"

class TestsNetwork final : public TRAP::Application
{
public:
	TestsNetwork()
	{
		PushLayer(TRAP::MakeScope<FTPTests>());
		//PushLayer(TRAP::MakeScope<SocketTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<TestsNetwork>();
}