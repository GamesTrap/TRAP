#include <TRAP.h>

#include "FTP/FTPTests.h" // IWYU pragma: keep
#include "Socket/SocketTests.h" // IWYU pragma: keep
#include "IPv6/IPv6Tests.h" // IWYU pragma: keep

class TestsNetwork final : public TRAP::Application
{
public:
	explicit TestsNetwork(std::string gameName)
		: Application(std::move(gameName))
	{
		// PushLayer(std::make_unique<FTPTests>());
		PushLayer(std::make_unique<SocketTests>());
		// PushLayer(std::make_unique<IPv6Tests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication([[maybe_unused]] const std::span<const std::string_view> args)
{
	return std::make_unique<TestsNetwork>("TestsNetwork");
}