#include <TRAP.h>

#include "Math/MathTests.h"

class UnitTestApp final : public TRAP::Application
{
public:
	explicit UnitTestApp(std::string gameName)
		: Application(std::move(gameName))
	{
		 PushLayer(std::make_unique<MathTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication(int32_t, char**)
{
	return std::make_unique<UnitTestApp>("UnitTests");
}