#include <TRAP.h>

#include "AntiAliasing/AntiAliasingTests.h" // IWYU pragma: keep
#include "Compute/ComputeTests.h" // IWYU pragma: keep
#include "Hash/HashTests.h" // IWYU pragma: keep
#include "Headless/HeadlessTests.h" // IWYU pragma: keep
#include "IPAddress/IPAddressTests.h" // IWYU pragma: keep

class HeadlessTestsApp final : public TRAP::Application
{
public:
	explicit HeadlessTestsApp(std::string gameName)
		: Application(std::move(gameName))
	{
		// PushLayer(std::make_unique<AntiAliasingTests>());
		PushLayer(std::make_unique<ComputeTests>());
		// PushLayer(std::make_unique<HeadlessTests>());
		// PushLayer(std::make_unique<HashTests>());
		// PushLayer(std::make_unique<IPAddressTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication([[maybe_unused]] const std::span<const std::string_view> args)
{
	return std::make_unique<HeadlessTestsApp>("HeadlessTests");
}