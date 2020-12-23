#include "TRAPPCH.h"
#include "VulkanRenderer.h"

#include "Application.h"
#include "VulkanCommon.h"
#include "Graphics/RenderCommand.h"
#include "Window/Window.h"
#include "Window/WindowingAPI.h"
#include "Utils/Utils.h"

#include "Objects/VulkanDescriptorPool.h"
#include "Objects/VulkanMemoryAllocator.h"
#include "Objects/VulkanDevice.h"
#include "Objects/VulkanPhysicalDevice.h"
#include "Objects/VulkanInstance.h"
#include "Objects/VulkanDebug.h"

TRAP::Graphics::API::VulkanRenderer* TRAP::Graphics::API::VulkanRenderer::s_renderer = nullptr;
//Instance Extensions
bool TRAP::Graphics::API::VulkanRenderer::s_debugUtilsExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_swapchainColorSpaceExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_VRExtensions = false;
//Device Extensions
bool TRAP::Graphics::API::VulkanRenderer::s_shaderDrawParameters = false;
bool TRAP::Graphics::API::VulkanRenderer::s_subgroupBroadcastDynamicID = false;
bool TRAP::Graphics::API::VulkanRenderer::s_fragmentShaderInterlockExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_drawIndirectCountExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_descriptorIndexingExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_raytracingExtension = false;

bool TRAP::Graphics::API::VulkanRenderer::s_renderdocCapture = false;
bool TRAP::Graphics::API::VulkanRenderer::s_debugMarkerSupport = false;

std::vector<std::pair<std::string, std::array<uint8_t, 16>>> TRAP::Graphics::API::VulkanRenderer::s_usableGPUs{};

TRAP::Scope<TRAP::Graphics::API::VulkanRenderer::NullDescriptors> TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors = nullptr;

TRAP::Graphics::API::VulkanRenderer::GPUCapBits TRAP::Graphics::API::VulkanRenderer::s_GPUCapBits{};

std::vector<VkPipelineColorBlendAttachmentState> TRAP::Graphics::API::VulkanRenderer::DefaultBlendAttachments(8);
VkPipelineRasterizationStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultRasterizerDesc = UtilToRasterizerDesc({CullMode::Back, {}, {}, {}, {}, {}, {}, {}});
VkPipelineDepthStencilStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultDepthDesc = UtilToDepthDesc({ false, false, CompareMode::LessOrEqual, {}, 0xFF, 0xFF, CompareMode::Always, {}, {}, {}, CompareMode::Always, {}, {}, {} });
VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultBlendDesc = UtilToBlendDesc({ {BlendConstant::One}, {BlendConstant::Zero}, {BlendConstant::One}, {BlendConstant::Zero}, {}, {}, {(0x1 | 0x2 | 0x4 | 0x8)}, BlendStateTargets::BlendStateTargetAll, {}, false }, DefaultBlendAttachments);

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::VulkanRenderer()
{
	s_renderer = this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::~VulkanRenderer()
{
	TP_DEBUG(Log::RendererVulkanPrefix, "Destroying Renderer");

	RemoveDefaultResources();

	//Free everything in order
	//Should happen automagically through Scope deconstructors
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::InitInternal()
{
	m_instance = TRAP::MakeRef<VulkanInstance>(Application::GetWindow()->GetTitle(), SetupInstanceLayers(), SetupInstanceExtensions());
#if defined(ENABLE_GRAPHICS_DEBUG)
	m_debug = TRAP::MakeScope<VulkanDebug>(m_instance);
#endif
	
	const std::multimap<uint32_t, std::array<uint8_t, 16>> physicalDevices = VulkanPhysicalDevice::GetAllRatedPhysicalDevices(m_instance);
	TRAP::Scope<VulkanPhysicalDevice> physicalDevice;
	
	//Get Vulkan GPU UUID
	const std::string UUIDstr = TRAP::Application::GetConfig().Get<std::string>("VulkanGPU");
	const std::array<uint8_t, 16> UUID = TRAP::Utils::UUIDFromString(UUIDstr);
	
	if(UUID == std::array<uint8_t, 16>{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0})
	{
		TP_ERROR(Log::RendererVulkanPrefix, "Invalid GPU UUID: \"", UUIDstr, "\"!");
		TP_ERROR(Log::RendererVulkanPrefix, "Falling back to score based system");
		physicalDevice = TRAP::MakeScope<VulkanPhysicalDevice>(m_instance, (--physicalDevices.end())->second);
	}
	else
	{
		for (const auto& [score, devUUID] : physicalDevices)
		{
			if(UUID == devUUID)
			{
				physicalDevice = TRAP::MakeScope<VulkanPhysicalDevice>(m_instance, devUUID);
				break;
			}
		}

		if(!physicalDevice)
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Could not find a GPU with UUID: \"", UUIDstr, "\"!");
			TP_ERROR(Log::RendererVulkanPrefix, "Falling back to score based system");
			physicalDevice = TRAP::MakeScope<VulkanPhysicalDevice>(m_instance, (--physicalDevices.end())->second);
		}
	}

	m_device = TRAP::MakeRef<VulkanDevice>(std::move(physicalDevice), SetupDeviceExtensions(physicalDevice));

	m_vma = TRAP::MakeRef<VulkanMemoryAllocator>(m_device, m_instance);

	m_descriptorPool = TRAP::MakeRef<VulkanDescriptorPool>(m_device, 8192);

	m_device->FindQueueFamilyIndices();

	AddDefaultResources();
	
	const VkPhysicalDeviceProperties devProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties();
	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");
	TP_INFO(Log::RendererVulkanPrefix, "Vulkan:");
	TP_INFO(Log::RendererVulkanPrefix, "Instance Version: ",
		VK_VERSION_MAJOR(VulkanInstance::GetInstanceVersion()), '.',
		VK_VERSION_MINOR(VulkanInstance::GetInstanceVersion()), '.',
		VK_VERSION_PATCH(VulkanInstance::GetInstanceVersion()));
	TP_INFO(Log::RendererVulkanPrefix, "Driver Vulkan Version: ", VK_VERSION_MAJOR(devProps.apiVersion), '.', VK_VERSION_MINOR(devProps.apiVersion), '.', VK_VERSION_PATCH(devProps.apiVersion));
	TP_INFO(Log::RendererVulkanPrefix, "Renderer: ", devProps.deviceName);
	TP_INFO(Log::RendererVulkanPrefix, "Driver: ", VK_VERSION_MAJOR(devProps.driverVersion), '.', VK_VERSION_MINOR(devProps.driverVersion), '.', VK_VERSION_PATCH(devProps.driverVersion));
	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");

	m_rendererTitle = "[Vulkan " + std::to_string(VK_VERSION_MAJOR(devProps.apiVersion)) + "." +
							       std::to_string(VK_VERSION_MINOR(devProps.apiVersion)) + "." +
								   std::to_string(VK_VERSION_PATCH(devProps.apiVersion)) + "]";
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Clear(RendererBufferType buffer)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Present(const Scope<Window>& window)
{
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetClearColor(const Math::Vec4& color)
{
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthTesting(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthMasking(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthFunction(RendererFunction function)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilTesting(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilMasking(uint32_t mask)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilMaskingSeparate(RendererFaceMode face, uint32_t mask)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilFunction(RendererFunction function, int32_t reference, uint32_t mask)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilFunctionSeparate(RendererFaceMode face, RendererFunction function, int32_t reference, uint32_t mask)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilOperation(RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilOperationSeparate(RendererFaceMode face, RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlend(const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetCull(const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetFrontFace(const RendererFrontFace frontFace)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetWireFrame(const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendFunctionSeparate(RendererBlendFunction sourceRGB,
	                                                               RendererBlendFunction sourceAlpha,
	                                                               RendererBlendFunction destinationRGB,
	                                                               RendererBlendFunction destinationAlpha)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendEquation(RendererBlendEquation blendEquation)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB, RendererBlendEquation blendEquationAlpha)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetCullMode(const RendererCullMode cullMode)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Draw(const Scope<VertexArray>& vertexArray)
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::API::VulkanRenderer::GetTitle() const
{
	return m_rendererTitle;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 16> TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUUUID()
{
	return m_device->GetPhysicalDevice()->GetPhysicalDeviceUUID();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUName()
{
	return m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().deviceName;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::pair<std::string, std::array<uint8_t, 16>>> TRAP::Graphics::API::VulkanRenderer::GetAllGPUs()
{
	if(s_usableGPUs.empty())
	{
		for(const auto& [score, devUUID] : VulkanPhysicalDevice::GetAllRatedPhysicalDevices(m_instance))
		{
			const VkPhysicalDevice dev = VulkanPhysicalDevice::FindPhysicalDeviceViaUUID(m_instance, devUUID);
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(dev, &props);

			s_usableGPUs.emplace_back(props.deviceName, devUUID);
		}
	}
	
	return s_usableGPUs;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Graphics::API::VulkanRenderer::SetupInstanceLayers()
{
	std::vector<std::string> layers{};
	
#ifdef ENABLE_GRAPHICS_DEBUG
	if (VulkanInstance::IsLayerSupported("VK_LAYER_KHRONOS_validation"))
		layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif

#ifdef USE_RENDER_DOC
	if(VulkanInstance::IsLayerSupported("VK_LAYER_RENDERDOC_Capture"))
	{
		layers.emplace_back("VK_LAYER_RENDERDOC_Capture");
		s_renderdocCapture = true;
	}
#endif

	return layers;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Graphics::API::VulkanRenderer::SetupInstanceExtensions()
{
	std::vector<std::string> extensions{};

	const auto reqExt = INTERNAL::WindowingAPI::GetRequiredInstanceExtensions();

	if(!VulkanInstance::IsExtensionSupported(reqExt[0]) || !VulkanInstance::IsExtensionSupported(reqExt[1]))
	{
		TP_CRITICAL(Log::RendererVulkanPrefix, "Mandatory Vulkan Surface extensions are unsupported! Trying to switch RenderAPI");
		Graphics::RendererAPI::SwitchRenderAPI(RenderAPI::NONE); //TODO Switch to D3D12 instead
	}
	else
	{
		extensions.push_back(reqExt[0]);
		extensions.push_back(reqExt[1]);
	}
	
#ifdef ENABLE_GRAPHICS_DEBUG
	if(VulkanInstance::IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		s_debugUtilsExtension = true;
	}
#endif

	///HDR support
	if (VulkanInstance::IsExtensionSupported(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
		s_swapchainColorSpaceExtension = true;
	}

	///VR support
	if (VulkanInstance::IsExtensionSupported(VK_KHR_DISPLAY_EXTENSION_NAME) &&
		VulkanInstance::IsExtensionSupported(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_DISPLAY_EXTENSION_NAME);
		extensions.emplace_back(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME);
		s_VRExtensions = true;
	}
	
	return extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Graphics::API::VulkanRenderer::SetupDeviceExtensions(const TRAP::Scope<VulkanPhysicalDevice>& physicalDevice)
{
	std::vector<std::string> extensions{};

	if(physicalDevice->IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
		extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	else
	{
		TP_CRITICAL(Log::RendererVulkanPrefix, "Mandatory Vulkan swapchain extension is unsupported! Trying to switch RenderAPI");
		Graphics::RendererAPI::SwitchRenderAPI(RenderAPI::NONE); //TODO Switch to D3D12 instead
	}

	if (physicalDevice->IsExtensionSupported(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME);
		s_drawIndirectCountExtension = true;
	}

	if (physicalDevice->IsExtensionSupported(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME);
		s_fragmentShaderInterlockExtension = true;
	}

	if (physicalDevice->IsExtensionSupported(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
		s_descriptorIndexingExtension = true;
	}

	if (physicalDevice->IsExtensionSupported(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) &&
		physicalDevice->IsExtensionSupported(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME) &&
		physicalDevice->IsExtensionSupported(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME) &&
		physicalDevice->IsExtensionSupported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) &&
		physicalDevice->IsExtensionSupported(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME) &&
		physicalDevice->IsExtensionSupported(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME) &&
		physicalDevice->IsExtensionSupported(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME))
	{
		extensions.emplace_back("VK_KHR_ray_tracing_pipeline");
		extensions.emplace_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
		extensions.emplace_back("VK_KHR_acceleration_structure");
		extensions.emplace_back("VK_KHR_deferred_host_operations");
		extensions.emplace_back("VK_KHR_pipeline_library");
		s_raytracingExtension = true;
	}

	return extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::AddDefaultResources()
{
	s_NullDescriptors = TRAP::MakeScope<struct NullDescriptors>();
	
	//TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::RemoveDefaultResources()
{
	//TODO
	
	s_NullDescriptors.reset();
	s_NullDescriptors = nullptr;
}