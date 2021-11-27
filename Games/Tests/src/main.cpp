#include <TRAP.h>

#include "Clipboard/ClipboardTests.h"
#include "Controllers/ControllerTests.h"
#include "Cursor/CursorTests.h"
#include "DragAndDrop/DragAndDropTests.h"
#include "Events/EventTests.h"
#include "Icon/IconTests.h"
#include "ImageLoader/ImageLoaderTests.h"
#include "Monitors/MonitorTests.h"
#include "Opacity/OpacityTests.h"
#include "Title/TitleTests.h"
#include "Vulkan/VulkanIcoSphereTests.h"
#include "Vulkan/VulkanMultiWindowTests.h"
#include "Vulkan/VulkanTests.h"
#include "Vulkan/VulkanTextureTests.h"
#include "WindowState/WindowStateTests.h"

class Tests final : public TRAP::Application
{
public:
	explicit Tests(std::string gameName)
		: Application(std::move(gameName))
	{
		// PushLayer(TRAP::MakeScope<ClipboardTests>());
		//PushLayer(TRAP::MakeScope<ControllerTests>());
		//PushLayer(TRAP::MakeScope<CursorTests>());
		// PushLayer(TRAP::MakeScope<DragAndDropTests>());
		//PushLayer(TRAP::MakeScope<EventTests>());
		//PushLayer(TRAP::MakeScope<IconTests>());
		// PushLayer(TRAP::MakeScope<ImageLoaderTests>());
		PushLayer(TRAP::MakeScope<MonitorTests>());
		//PushLayer(TRAP::MakeScope<OpacityTests>());
		//PushLayer(TRAP::MakeScope<TitleTests>());
		// PushLayer(TRAP::MakeScope<VulkanIcoSphereTests>());
		// PushLayer(TRAP::MakeScope<VulkanMultiWindowTests>());
		// PushLayer(TRAP::MakeScope<VulkanTests>());
		// PushLayer(TRAP::MakeScope<VulkanTextureTests>());
		//PushLayer(TRAP::MakeScope<WindowStateTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<Tests>("Tests");
}