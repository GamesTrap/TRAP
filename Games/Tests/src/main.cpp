#include <TRAP.h>

#include "Clipboard/ClipboardTests.h"
#include "Cursor/CursorTests.h"
#include "Events/EventTests.h"
#include "WindowState/WindowStateTests.h"
#include "Controllers/ControllerTests.h"
#include "Icon/IconTests.h"
#include "Monitors/MonitorTests.h"
#include "Opacity/OpacityTests.h"
#include "Title/TitleTests.h"
#include "Vulkan/VulkanTests.h"
#include "ImageLoader/ImageLoaderTests.h"
#include "DragAndDrop/DragAndDropTests.h"

class Tests final : public TRAP::Application
{
public:
	Tests()
	{
		PushLayer(TRAP::MakeScope<ClipboardTests>());
		//PushLayer(TRAP::MakeScope<CursorTests>());
		//PushLayer(TRAP::MakeScope<EventTests>());
		//PushLayer(TRAP::MakeScope<WindowStateTests>());
		//PushLayer(TRAP::MakeScope<ControllerTests>());
		//PushLayer(TRAP::MakeScope<IconTests>());
		//PushLayer(TRAP::MakeScope<MonitorTests>());
		//PushLayer(TRAP::MakeScope<OpacityTests>());
		//PushLayer(TRAP::MakeScope<TitleTests>());
		//PushLayer(TRAP::MakeScope<VulkanTests>());
		//PushLayer(TRAP::MakeScope<ImageLoaderTests>());
		//PushLayer(TRAP::MakeScope<DragAndDropTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<Tests>();
}