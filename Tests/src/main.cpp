#include <TRAP.h>

#include "Clipboard/Clipboard.h"
#include "Cursor/Cursor.h"

class Tests final : public TRAP::Application
{
public:
	Tests()
	{
		//PushLayer(TRAP::MakeScope<Clipboard>());
		PushLayer(TRAP::MakeScope<Cursor>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<Tests>();
}