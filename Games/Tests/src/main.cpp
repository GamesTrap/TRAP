#include <TRAP.h>

#include "Anisotropy/AnisotropyTests.h" // IWYU pragma: keep
#include "AntiAliasing/AntiAliasingTests.h" // IWYU pragma: keep
#include "Clipboard/ClipboardTests.h" // IWYU pragma: keep
#include "Controllers/ControllerTests.h" // IWYU pragma: keep
#include "Compute/ComputeTests.h" // IWYU pragma: keep
#include "Cursor/CursorTests.h" // IWYU pragma: keep
#include "DragAndDrop/DragAndDropTests.h" // IWYU pragma: keep
#include "Events/EventTests.h" // IWYU pragma: keep
#include "FileSystem/FileSystemTests.h" // IWYU pragma: keep
#include "FrameBuffer/FrameBufferTests.h" // IWYU pragma: keep
#include "Icon/IconTests.h" // IWYU pragma: keep
#include "IcoSphere/IcoSphereTests.h" // IWYU pragma: keep
#include "ImageLoader/ImageLoaderTests.h" // IWYU pragma: keep
#include "InputLag/InputLagTests.h" // IWYU pragma: keep
#include "Monitors/MonitorTests.h" // IWYU pragma: keep
#include "MultiWindow/MultiWindowTests.h" // IWYU pragma: keep
#include "Opacity/OpacityTests.h" // IWYU pragma: keep
#include "RendererAPI/RendererAPIInfo.h" // IWYU pragma: keep
#include "RendererAPI/RendererAPITests.h" // IWYU pragma: keep
#include "RenderScale/RenderScaleTests.h" // IWYU pragma: keep
#include "Screenshot/ScreenshotTests.h" // IWYU pragma: keep
#include "SPIRV/SPIRVTests.h" // IWYU pragma: keep
#include "SpriteSheet/SpriteSheetTests.h" // IWYU pragma: keep
#include "Title/TitleTests.h" // IWYU pragma: keep
#include "VariableRateShading/VRSTests.h" // IWYU pragma: keep
#include "Vulkan/VulkanTextureTests.h" // IWYU pragma: keep
#include "WindowState/WindowStateTests.h" // IWYU pragma: keep
#include "WindowFeatures/WindowFeaturesTests.h" // IWYU pragma: keep

class Tests final : public TRAP::Application
{
public:
	explicit Tests(std::string gameName)
		: Application(std::move(gameName))
	{
		// PushLayer(std::make_unique<AnisotropyTests>());
		// PushLayer(std::make_unique<AntiAliasingTests>());
		// PushLayer(std::make_unique<ClipboardTests>());
		// PushLayer(std::make_unique<ControllerTests>());
		// PushLayer(std::make_unique<ComputeTests>());
		// PushLayer(std::make_unique<CursorTests>());
		// PushLayer(std::make_unique<DragAndDropTests>());
		// PushLayer(std::make_unique<EventTests>());
		// PushLayer(std::make_unique<FileSystemTests>());
		// PushLayer(std::make_unique<FrameBufferTests>());
		// PushLayer(std::make_unique<IconTests>());
		// PushLayer(std::make_unique<IcoSphereTests>());
		// PushLayer(std::make_unique<ImageLoaderTests>());
		// PushLayer(std::make_unique<InputLagTests>());
		// PushLayer(std::make_unique<MonitorTests>());
		// PushLayer(std::make_unique<MultiWindowTests>());
		// PushLayer(std::make_unique<OpacityTests>());
		// PushLayer(std::make_unique<RendererAPIInfo>());
		// PushLayer(std::make_unique<RendererAPITests>());
		PushLayer(std::make_unique<RenderScaleTests>());
		// PushLayer(std::make_unique<ScreenshotTests>());
		// PushLayer(std::make_unique<SPIRVTests>());
		// PushLayer(std::make_unique<SpriteSheetTests>());
		// PushLayer(std::make_unique<TitleTests>());
		// PushLayer(std::make_unique<VRSTests>());
		// PushLayer(std::make_unique<VulkanTextureTests>());
		// PushLayer(std::make_unique<WindowStateTests>());
		// PushLayer(std::make_unique<WindowFeaturesTests>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication([[maybe_unused]] const std::span<const std::string_view> args)
{
	return std::make_unique<Tests>("Tests");
}