#include <TRAP.h>

#include "AntiAliasing/AntiAliasingTests.h"
#include "Compute/ComputeTests.h"
#include "Headless/HeadlessTests.h"
#include "IPAddress/IPAddressTests.h"
#include "Math/MathTests.h"

class HeadlessTestsApp final : public TRAP::Application
{
public:
	explicit HeadlessTestsApp(std::string gameName)
		: Application(std::move(gameName))
	{
		// PushLayer(std::make_unique<AntiAliasingTests>());
		// PushLayer(std::make_unique<ComputeTests>());
		// PushLayer(std::make_unique<HeadlessTests>());
		PushLayer(std::make_unique<IPAddressTests>());
		// PushLayer(std::make_unique<MathTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication(int32_t, char**)
{
	return std::make_unique<HeadlessTestsApp>("HeadlessTests");
}