#include <TRAP.h>

#include "Clipboard/ClipboardTests.h"
#include "Controllers/ControllerTests.h"
#include "Cursor/CursorTests.h"
#include "DragAndDrop/DragAndDropTests.h"
#include "Events/EventTests.h"
#include "FileSystem/FileSystemTests.h"
#include "Icon/IconTests.h"
#include "ImageLoader/ImageLoaderTests.h"
#include "Monitors/MonitorTests.h"
#include "Opacity/OpacityTests.h"
#include "RendererAPI/RendererAPIInfo.h"
#include "Screenshot/ScreenshotTests.h"
#include "Title/TitleTests.h"
#include "Vulkan/VulkanFrameBufferTests.h"
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
		// PushLayer(std::make_unique<ClipboardTests>());
		// PushLayer(std::make_unique<ControllerTests>());
		// PushLayer(std::make_unique<CursorTests>());
		// PushLayer(std::make_unique<DragAndDropTests>());
		// PushLayer(std::make_unique<EventTests>());
		// PushLayer(std::make_unique<FileSystemTests>());
		// PushLayer(std::make_unique<IconTests>());
		// PushLayer(std::make_unique<ImageLoaderTests>());
		// PushLayer(std::make_unique<MonitorTests>());
		// PushLayer(std::make_unique<OpacityTests>());
		PushLayer(std::make_unique<RendererAPIInfo>());
		// PushLayer(std::make_unique<ScreenshotTests>());
		// PushLayer(std::make_unique<TitleTests>());
		// PushLayer(std::make_unique<VulkanFrameBufferTests>());
		// PushLayer(std::make_unique<VulkanIcoSphereTests>());
		// PushLayer(std::make_unique<VulkanMultiWindowTests>());
		// PushLayer(std::make_unique<VulkanTests>());
		// PushLayer(std::make_unique<VulkanTextureTests>());
		// PushLayer(std::make_unique<WindowStateTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication(int32_t, char**)
{
	return std::make_unique<Tests>("Tests");
}