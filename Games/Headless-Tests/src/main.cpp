#include <TRAP.h>

#include "Headless/HeadlessTests.h"

class HeadlessTestsApp final : public TRAP::Application
{
public:
	explicit HeadlessTestsApp(std::string gameName)
		: Application(std::move(gameName))
	{
		PushLayer(std::make_unique<HeadlessTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<HeadlessTestsApp>("HeadlessTests");
}