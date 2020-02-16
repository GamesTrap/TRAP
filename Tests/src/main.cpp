#include <TRAP.h>

#include "Clipboard/ClipboardTests.h"
#include "Cursor/CursorTests.h"
#include "Events/EventTests.h"

class Tests final : public TRAP::Application
{
public:
	Tests()
	{
		//PushLayer(TRAP::MakeScope<ClipboardTests>());
		//PushLayer(TRAP::MakeScope<CursorTests>());
		PushLayer(TRAP::MakeScope<EventTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<Tests>();
}