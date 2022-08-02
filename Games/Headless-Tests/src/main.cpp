#include <TRAP.h>

#include "AntiAliasing/AntiAliasingTests.h"
#include "Headless/HeadlessTests.h"
#include "Compute/ComputeTests.h"

class HeadlessTestsApp final : public TRAP::Application
{
public:
	explicit HeadlessTestsApp(std::string gameName)
		: Application(std::move(gameName))
	{
		PushLayer(std::make_unique<AntiAliasingTests>());
		// PushLayer(std::make_unique<ComputeTests>());
		// PushLayer(std::make_unique<HeadlessTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication(int32_t, char**)
{
	return std::make_unique<HeadlessTestsApp>("HeadlessTests");
}