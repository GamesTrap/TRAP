#include <TRAP.h>

#include "Clipboard/ClipboardTests.h"
#include "Cursor/CursorTests.h"
#include "Events/EventTests.h"
#include "Minimize/MinimizeTests.h"
#include "Controllers/ControllerTests.h"
#include "Icon/IconTests.h"
#include "Monitors/MonitorTests.h"
#include "Opacity/OpacityTests.h"

class Tests final : public TRAP::Application
{
public:
	Tests()
	{
		//PushLayer(TRAP::MakeScope<ClipboardTests>());
		//PushLayer(TRAP::MakeScope<CursorTests>());
		//PushLayer(TRAP::MakeScope<EventTests>());
		//PushLayer(TRAP::MakeScope<MinimizeTests>());
		//PushLayer(TRAP::MakeScope<ControllerTests>());
		//PushLayer(TRAP::MakeScope<IconTests>());
		//PushLayer(TRAP::MakeScope<MonitorTests>());
		PushLayer(TRAP::MakeScope<OpacityTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<Tests>();
}