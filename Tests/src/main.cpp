#include <TRAP.h>

#include "Clipboard/ClipboardTests.h"
#include "Cursor/CursorTests.h"

class Tests final : public TRAP::Application
{
public:
	Tests()
	{
		//PushLayer(TRAP::MakeScope<ClipboardTests>());
		PushLayer(TRAP::MakeScope<CursorTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<Tests>();
}