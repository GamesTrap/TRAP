#include "TRAPPCH.h"
#include "VulkanRenderer.h"

#include "Application.h"
#include "VulkanCommon.h"
#include "Graphics/RenderCommand.h"
#include "Window/Window.h"
#include "Window/WindowingAPI.h"
#include "Utils/Utils.h"

#include <Graphics/API/Objects/Pipeline.h>

#include "Objects/VulkanSemaphore.h"
#include "Objects/VulkanFence.h"
#include "Objects/VulkanSampler.h"
#include "Objects/VulkanBuffer.h"
#include "Objects/VulkanTexture.h"
#include "Objects/VulkanCommandPool.h"
#include "Objects/VulkanCommandBuffer.h"
#include "Objects/VulkanDescriptorPool.h"
#include "Objects/VulkanMemoryAllocator.h"
#include "Objects/VulkanDevice.h"
#include "Objects/VulkanPhysicalDevice.h"
#include "Objects/VulkanInstance.h"
#include "Objects/VulkanDebug.h"

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/Objects/RenderTarget.h"
#include "Graphics/API/Objects/RootSignature.h"
#include "Graphics/Shaders/Shader.h"

#include "Graphics/API/PipelineDescHash.h"

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
bool TRAP::Graphics::API::VulkanRenderer::s_samplerYcbcrConversionExtension = false;

bool TRAP::Graphics::API::VulkanRenderer::s_renderdocCapture = false;
bool TRAP::Graphics::API::VulkanRenderer::s_debugMarkerSupport = false;

std::vector<std::pair<std::string, std::array<uint8_t, 16>>> TRAP::Graphics::API::VulkanRenderer::s_usableGPUs{};

TRAP::Scope<TRAP::Graphics::API::VulkanRenderer::NullDescriptors> TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors = nullptr;

TRAP::Graphics::API::VulkanRenderer::GPUCapBits TRAP::Graphics::API::VulkanRenderer::s_GPUCapBits{};

std::vector<VkPipelineColorBlendAttachmentState> TRAP::Graphics::API::VulkanRenderer::DefaultBlendAttachments(8);
VkPipelineRasterizationStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultRasterizerDesc = UtilToRasterizerDesc({CullMode::Back, {}, {}, {}, {}, {}});
VkPipelineDepthStencilStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultDepthDesc = UtilToDepthDesc({ false, false, CompareMode::LessOrEqual, {}, 0xFF, 0xFF, CompareMode::Always, {}, {}, {}, CompareMode::Always, {}, {}, {} });
VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultBlendDesc = UtilToBlendDesc({ {BlendConstant::One}, {BlendConstant::Zero}, {BlendConstant::One}, {BlendConstant::Zero}, {}, {}, {(0x1 | 0x2 | 0x4 | 0x8)}, BlendStateTargets::BlendStateTargetAll, false }, DefaultBlendAttachments);

TRAP::Scope<std::unordered_map<std::thread::id, TRAP::Graphics::API::VulkanRenderer::RenderPassMap>> TRAP::Graphics::API::VulkanRenderer::s_renderPassMap = TRAP::MakeScope<std::unordered_map<std::thread::id, RenderPassMap>>();
TRAP::Scope<std::unordered_map<std::thread::id, TRAP::Graphics::API::VulkanRenderer::FrameBufferMap>> TRAP::Graphics::API::VulkanRenderer::s_frameBufferMap = TRAP::MakeScope<std::unordered_map<std::thread::id, FrameBufferMap>>();
std::mutex TRAP::Graphics::API::VulkanRenderer::s_renderPassMutex{};

std::unordered_map<uint64_t, TRAP::Ref<TRAP::Graphics::Pipeline>> TRAP::Graphics::API::VulkanRenderer::s_pipelines{};
std::mutex TRAP::Graphics::API::VulkanRenderer::s_pipelineMutex{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::VulkanRenderer()
	: m_instance(nullptr),
	  m_debug(nullptr),
	  m_device(nullptr),
	  m_vma(nullptr),
	  m_descriptorPool(nullptr),
	  m_vsyncNew(false)
{
	s_renderer = this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::~VulkanRenderer()
{
	TP_DEBUG(Log::RendererVulkanPrefix, "Destroying Renderer");

	s_ResourceLoader.reset();
	
	RemoveDefaultResources();

	std::lock_guard<std::mutex> lock(s_renderPassMutex);
	s_frameBufferMap.reset();
	
	s_renderPassMap.reset();

	s_pipelines.clear();

	//Free everything in order
	//Should happen automagically through Scope destructors
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::StartGraphicRecording(const TRAP::Scope<PerWindowData>& p)
{
	TRAP_ASSERT(p);

	if (!p->Window->IsMinimized() && !p->Recording)
	{
		//Start Recording
		p->CurrentSwapChainImageIndex = p->SwapChain->AcquireNextImage(p->ImageAcquiredSemaphore, nullptr);

		const TRAP::Ref<RenderTarget> renderTarget = p->SwapChain->GetRenderTargets()[p->CurrentSwapChainImageIndex];
		const TRAP::Ref<Semaphore> renderCompleteSemaphore = p->RenderCompleteSemaphores[p->ImageIndex];
		TRAP::Ref<Fence> renderCompleteFence = p->RenderCompleteFences[p->ImageIndex];

		//Stall if CPU is running "Swap Chain Buffer Count" frames ahead of GPU
		if (renderCompleteFence->GetStatus() == FenceStatus::Incomplete)
			renderCompleteFence->Wait();

		//Reset cmd pool for this frame
		p->GraphicCommandPools[p->ImageIndex]->Reset();

		p->GraphicCommandBuffers[p->ImageIndex]->Begin();

		const RenderTargetBarrier barrier = { renderTarget, ResourceState::Present, ResourceState::RenderTarget };
		p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier({}, {}, { barrier });

		LoadActionsDesc loadActions{};
		loadActions.LoadActionsColor[0] = LoadActionType::Clear;
		loadActions.ClearColorValues[0] = p->ClearColor;
		p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({ renderTarget }, nullptr, &loadActions, {}, {}, -1, -1);

		//Set Default Dynamic Viewport & Scissor
		p->GraphicCommandBuffers[p->ImageIndex]->SetViewport(0.0f, 0.0f, static_cast<float>(p->Window->GetWidth()), static_cast<float>(p->Window->GetHeight()), 0.0f, 1.0f);
		p->GraphicCommandBuffers[p->ImageIndex]->SetScissor(0, 0, p->Window->GetWidth(), p->Window->GetHeight());
		if(p->CurrentGraphicPipeline)
			p->GraphicCommandBuffers[p->ImageIndex]->BindPipeline(p->CurrentGraphicPipeline);
		//TODO Also check if pipeline changed?!
		
		p->Recording = true;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::EndGraphicRecording(const TRAP::Scope<PerWindowData>& p)
{
	if (!p->Window->IsMinimized() && p->Recording)
	{
		//End Recording
		p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({}, nullptr, nullptr, {}, {}, -1, -1);

		const RenderTargetBarrier barrier = { p->SwapChain->GetRenderTargets()[p->CurrentSwapChainImageIndex], ResourceState::RenderTarget, ResourceState::Present };
		p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier({}, {}, { barrier });

		p->GraphicCommandBuffers[p->ImageIndex]->End();

		QueueSubmitDesc submitDesc{};
		submitDesc.Cmds = { p->GraphicCommandBuffers[p->ImageIndex] };
		submitDesc.SignalSemaphores = { p->RenderCompleteSemaphores[p->ImageIndex] };
		submitDesc.WaitSemaphores = { p->ImageAcquiredSemaphore };
		submitDesc.SignalFence = p->RenderCompleteFences[p->ImageIndex];
		p->GraphicQueue->Submit(submitDesc);

		QueuePresentDesc presentDesc{};
		presentDesc.Index = static_cast<uint8_t>(p->CurrentSwapChainImageIndex);
		presentDesc.WaitSemaphores = { p->RenderCompleteSemaphores[p->ImageIndex] };
		presentDesc.SwapChain = p->SwapChain;
		const PresentStatus presentStatus = p->GraphicQueue->Present(presentDesc);
		if (presentStatus == PresentStatus::DeviceReset || presentStatus == PresentStatus::Failed)
			TRAP::Application::Shutdown();
		else if (presentStatus == PresentStatus::OutOfDate)
		{
			//Resize needed
			//Clear SwapChain reference
			presentDesc = {};

			p->SwapChain.reset();

			SwapChainDesc swapChainDesc{};
			swapChainDesc.Window = p->Window;
			swapChainDesc.PresentQueues = { p->GraphicQueue };
			swapChainDesc.Width = p->Window->GetWidth();
			swapChainDesc.Height = p->Window->GetHeight();
			swapChainDesc.ImageCount = PerWindowData::ImageCount;
			swapChainDesc.ColorFormat = SwapChain::GetRecommendedSwapchainFormat(true);
			swapChainDesc.ColorClearValue = p->ClearColor;
			swapChainDesc.EnableVSync = p->CurrentVSync;
			p->SwapChain = SwapChain::Create(swapChainDesc);

			if (!p->SwapChain)
				TRAP::Application::Shutdown();
		}

		p->ImageIndex = (p->ImageIndex + 1) % PerWindowData::ImageCount;

		p->Recording = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::InitInternal(const std::string& gameName)
{
	m_instance = TRAP::MakeRef<VulkanInstance>(gameName, SetupInstanceLayers(), SetupInstanceExtensions());
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

	m_descriptorPool = TRAP::MakeRef<VulkanDescriptorPool>(8192);

	m_device->FindQueueFamilyIndices();

	AddDefaultResources();

	s_ResourceLoader = TRAP::MakeScope<ResourceLoader>();
	
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

void TRAP::Graphics::API::VulkanRenderer::Present(const Scope<Window>& window)
{
	const TRAP::Scope<PerWindowData>& p = (*s_perWindowDataMap)[window.get()];

	EndGraphicRecording(p);
	if (p->CurrentVSync != m_vsyncNew)
	{
		std::lock_guard<std::mutex> lock(s_perWindowDataMutex);
		if(p->SwapChain)
			p->SwapChain->ToggleVSync();
		p->CurrentVSync = m_vsyncNew;
	}
	StartGraphicRecording(p);
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetVSync(const bool vsync, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();
	
	(*s_perWindowDataMap)[window]->NewVSync = vsync;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetClearColor(const Math::Vec4& color, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();
	
	(*s_perWindowDataMap)[window]->ClearColor = { color.x, color.y, color.z, color.w };
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthTesting(const bool enabled, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline).DepthState->DepthTest = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthWriting(const bool enabled, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline).DepthState->DepthWrite = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthFunction(const CompareMode function, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline).DepthState->DepthFunc = function;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthFail(const StencilOp front, const StencilOp back, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline);
	
	gpd.DepthState->DepthFrontFail = front;
	gpd.DepthState->DepthBackFail = back;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthBias(const int32_t depthBias, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline).RasterizerState->DepthBias = depthBias;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthBiasSlopeFactor(const float factor, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline).RasterizerState->SlopeScaledDepthBias = factor;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilTesting(const bool enabled, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline).DepthState->StencilTest = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilFail(const StencilOp front, const StencilOp back, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline);
	
	gpd.DepthState->StencilFrontFail = front;
	gpd.DepthState->StencilBackFail = back;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilPass(const StencilOp front, const StencilOp back, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline);
	
	gpd.DepthState->StencilFrontPass = front;
	gpd.DepthState->StencilBackPass = back;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilFunction(const CompareMode front, const CompareMode back, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline);

	gpd.DepthState->StencilFrontFunc = front;
	gpd.DepthState->StencilBackFunc = back;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilMask(const uint8_t read, const uint8_t write, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline);
	
	gpd.DepthState->StencilReadMask = read;
	gpd.DepthState->StencilWriteMask = write;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetCullMode(const CullMode mode, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline).RasterizerState->CullMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetFillMode(const FillMode mode, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& p = (*s_perWindowDataMap)[window];
	
	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RasterizerState->FillMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetFrontFace(const FrontFace face, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline).RasterizerState->FrontFace = face;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline);
	
	gpd.BlendState->BlendModes = { modeRGB };
	gpd.BlendState->BlendAlphaModes = { modeAlpha };
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendConstant(const BlendConstant sourceRGB,
                                                           const BlendConstant sourceAlpha,
                                                           const BlendConstant destinationRGB,
                                                           const BlendConstant destinationAlpha,
                                                           Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>((*s_perWindowDataMap)[window]->GraphicsPipelineDesc.Pipeline);
	
	gpd.BlendState->SrcFactors = {sourceRGB};
	gpd.BlendState->DstFactors = {destinationRGB};
	gpd.BlendState->SrcAlphaFactors = {sourceAlpha};
	gpd.BlendState->DstAlphaFactors = {destinationAlpha};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Clear(const ClearFlags clear, const ClearValue value, Window* window)
{
	if ((static_cast<uint32_t>(clear & ClearFlags::Color) && static_cast<uint32_t>(clear & ClearFlags::Depth)) ||
		(static_cast<uint32_t>(clear & ClearFlags::Color) && static_cast<uint32_t>(clear & ClearFlags::Stencil)))
	{
		TP_ERROR(Log::RendererVulkanPrefix, "ClearFlags must be Color or a combination of Depth/Stencil!");
		return;
	}

	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& data = (*s_perWindowDataMap)[window];
	const TRAP::Ref<RenderTarget>& renderTarget = data->SwapChain->GetRenderTargets()[data->ImageIndex];
	
	data->GraphicCommandBuffers[data->ImageIndex]->Clear(clear, value, renderTarget->GetWidth(), renderTarget->GetHeight());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetViewport(const uint32_t x,
                                                      const uint32_t y,
                                                      const uint32_t width,
                                                      const uint32_t height,
                                                      const float minDepth,
                                                      const float maxDepth,
													  Window* window)
{
	if (width == 0 || height == 0)
		return;
	
	if (!window)
		window = TRAP::Application::GetWindow().get();
	
	const TRAP::Scope<PerWindowData>& data = (*s_perWindowDataMap)[window];

	data->GraphicCommandBuffers[data->ImageIndex]->SetViewport(static_cast<const float>(x),
	                                                           static_cast<const float>(y),
	                                                           static_cast<const float>(width),
	                                                           static_cast<const float>(height),
	                                                           minDepth,
	                                                           maxDepth);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();
	
	const TRAP::Scope<PerWindowData>& data = (*s_perWindowDataMap)[window];

	data->GraphicCommandBuffers[data->ImageIndex]->SetScissor(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Draw(const uint32_t vertexCount, const uint32_t firstVertex, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& p = (*s_perWindowDataMap)[window];
	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);
	RootSignatureDesc& rsd = p->RootSignatureDesc;;
	
	//Create/Load Graphics Pipeline
	if(!gpd.RootSignature || std::find(rsd.Shaders.begin(), rsd.Shaders.end(), gpd.ShaderProgram) == rsd.Shaders.end())
	{
		rsd.Shaders.push_back(gpd.ShaderProgram);
		gpd.RootSignature = RootSignature::Create(rsd);
	}
	
	p->CurrentGraphicPipeline = GetPipeline(p->GraphicsPipelineDesc);
	p->GraphicCommandBuffers[p->ImageIndex]->BindPipeline(p->CurrentGraphicPipeline);
	
	p->GraphicCommandBuffers[p->ImageIndex]->Draw(vertexCount, firstVertex);
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

bool TRAP::Graphics::API::VulkanRenderer::GetVSync(Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();
	
	return (*s_perWindowDataMap)[window]->CurrentVSync;
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

void TRAP::Graphics::API::VulkanRenderer::InitPerWindowData(Window* window)
{
	std::lock_guard<std::mutex> lock(s_perWindowDataMutex);
	if (s_perWindowDataMap->find(window) == s_perWindowDataMap->end())
	{
		//Add new Window to map
		TRAP::Scope<PerWindowData> p = TRAP::MakeScope<PerWindowData>();

		p->Window = window;
		
		//Create Graphic Queue
		QueueDesc queueDesc{};
		queueDesc.Type = QueueType::Graphics;
		queueDesc.Flag = QueueFlag::InitMicroprofile;
		p->GraphicQueue = Queue::Create(queueDesc);

		//Create Compute Queue
		queueDesc.Type = QueueType::Compute;
		p->ComputeQueue = Queue::Create(queueDesc);

		//For each buffered image
		for (uint32_t i = 0; i < PerWindowData::ImageCount; ++i)
		{
			//Create Graphic Command Pool
			p->GraphicCommandPools[i] = CommandPool::Create({ p->GraphicQueue, false });
			//Allocate Graphic Command Buffer
			p->GraphicCommandBuffers[i] = p->GraphicCommandPools[i]->AllocateCommandBuffer(false);

			//Create Compute Command Pool
			p->ComputeCommandPools[i] = CommandPool::Create({ p->ComputeQueue, false });
			//Allocate Compute Command Buffer
			p->ComputeCommandBuffers[i] = p->ComputeCommandPools[i]->AllocateCommandBuffer(false);

			//Create Render Fences/Semaphores
			p->RenderCompleteFences[i] = Fence::Create();
			p->RenderCompleteSemaphores[i] = Semaphore::Create();
		}

		//Image Acquire Semaphore
		p->ImageAcquiredSemaphore = Semaphore::Create();

		if (!p->Window->IsMinimized())
		{
			//Create Swapchain
			p->CurrentVSync = window->GetVSync();
			SwapChainDesc swapChainDesc{};
			swapChainDesc.Window = window;
			swapChainDesc.PresentQueues = { p->GraphicQueue };
			swapChainDesc.Width = window->GetWidth();
			swapChainDesc.Height = window->GetHeight();
			swapChainDesc.ImageCount = PerWindowData::ImageCount;
			swapChainDesc.ColorFormat = SwapChain::GetRecommendedSwapchainFormat(true);
			swapChainDesc.EnableVSync = p->CurrentVSync;
			swapChainDesc.ColorClearValue = p->ClearColor;
			p->SwapChain = SwapChain::Create(swapChainDesc);

			if (!p->SwapChain)
				TRAP::Application::Shutdown();

			StartGraphicRecording(p);
		}

		//GraphicsPipeline
		const std::vector<TRAP::Ref<RenderTarget>>& rT = p->SwapChain->GetRenderTargets();
		p->GraphicsPipelineDesc = {};
		p->GraphicsPipelineDesc.Type = PipelineType::Graphics;
		p->GraphicsPipelineDesc.Pipeline = GraphicsPipelineDesc();
		p->RootSignatureDesc = {};
		GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);

		gpd.PrimitiveTopology = PrimitiveTopology::TriangleList;
		gpd.RenderTargetCount = 1;
		gpd.ColorFormats = { rT[0]->GetImageFormat(), rT[1]->GetImageFormat(), rT[2]->GetImageFormat() };
		gpd.SampleCount = rT[0]->GetSampleCount();
		gpd.SampleQuality = rT[0]->GetSampleQuality();

		gpd.DepthState = TRAP::MakeRef<DepthStateDesc>();
		gpd.DepthState->DepthTest = false;
		gpd.DepthState->DepthWrite = false;
		gpd.DepthState->DepthFunc = CompareMode::LessOrEqual;
		gpd.DepthState->StencilTest = false;
		gpd.DepthState->StencilReadMask = 0xFF;
		gpd.DepthState->StencilWriteMask = 0xFF;
		gpd.DepthState->StencilFrontFunc = CompareMode::Always;
		gpd.DepthState->StencilFrontFail = {};
		gpd.DepthState->DepthFrontFail = {};
		gpd.DepthState->StencilFrontPass = {};
		gpd.DepthState->StencilBackFunc = CompareMode::Always;
		gpd.DepthState->StencilBackFail = {};
		gpd.DepthState->DepthBackFail = {};
		gpd.DepthState->StencilBackPass = {};

		gpd.RasterizerState = TRAP::MakeRef<RasterizerStateDesc>();
		gpd.RasterizerState->CullMode = CullMode::Back;
		gpd.RasterizerState->DepthBias = 0;
		gpd.RasterizerState->SlopeScaledDepthBias = 0.0f;
		gpd.RasterizerState->FillMode = FillMode::Solid;
		gpd.RasterizerState->FrontFace = FrontFace::Clockwise;
		gpd.RasterizerState->DepthClampEnable = false;

		gpd.BlendState = TRAP::MakeRef<BlendStateDesc>();
		gpd.BlendState->SrcFactors = {BlendConstant::One };
		gpd.BlendState->DstFactors = {BlendConstant::Zero };
		gpd.BlendState->SrcAlphaFactors = { BlendConstant::One };
		gpd.BlendState->DstAlphaFactors = { BlendConstant::Zero};
		gpd.BlendState->BlendModes = {};
		gpd.BlendState->BlendAlphaModes = {};
		gpd.BlendState->Masks = {(0x1 | 0x2 | 0x4 | 0x8)};
		gpd.BlendState->RenderTargetMask = BlendStateTargets::BlendStateTargetAll;
		gpd.BlendState->IndependentBlend = false;

		gpd.VertexLayout = TRAP::MakeRef<VertexLayout>();
		const TRAP::Ref<VertexLayout>& vLayout = gpd.VertexLayout;
		vLayout->AttributeCount = 1;
		vLayout->Attributes[0].Format = ImageFormat::R32G32B32_SFLOAT;
		vLayout->Attributes[0].Binding = 0;
		vLayout->Attributes[0].Location = 0;
		vLayout->Attributes[0].Offset = 0;

		(*s_perWindowDataMap)[window] = std::move(p);
	}

	//Window is already in map
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::RemovePerWindowData(Window* window)
{
	std::lock_guard<std::mutex> lock(s_perWindowDataMutex);
	if (s_perWindowDataMap->find(window) != s_perWindowDataMap->end())
		(*s_perWindowDataMap).erase(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindShader(Shader* shader, Window* window) const
{
	if (!window)
		window = TRAP::Application::GetWindow().get();
	
	const TRAP::Scope<PerWindowData>& p = (*s_perWindowDataMap)[window];

	const ShaderStage stages = shader->GetShaderStages();

	if(stages == ShaderStage::RayTracing)
	{
		//TODO RayTracingPipelineDesc.Pipeline.ShaderProgram = shader;
		//Bind pipeline
	}
	else if(stages == ShaderStage::Compute)
	{
		//TODO ComputePipelineDesc.Pipeline.ShaderProgram = shader;
		//Bind pipeline
	}
	else if(stages != ShaderStage::None && stages != ShaderStage::SHADER_STAGE_COUNT)
	{
		GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);
		if(gpd.ShaderProgram != shader)
		{
			gpd.ShaderProgram = shader;

			RootSignatureDesc rsd{};
			rsd.Shaders = { shader };
			gpd.RootSignature = RootSignature::Create(rsd);

			p->CurrentGraphicPipeline = GetPipeline(p->GraphicsPipelineDesc);
			p->GraphicCommandBuffers[p->ImageIndex]->BindPipeline(p->CurrentGraphicPipeline);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandBuffer* TRAP::Graphics::API::VulkanRenderer::GetCurrentGraphicCommandBuffer(Window* window)
{
	auto it = s_perWindowDataMap->find(window);
	if (it != s_perWindowDataMap->end())
	{
		if (it->second->Window->IsMinimized())
			return nullptr;
		
		return it->second->GraphicCommandBuffers[it->second->ImageIndex];
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SwapChain> TRAP::Graphics::API::VulkanRenderer::GetCurrentSwapChain(Window* window)
{
	const auto it = s_perWindowDataMap->find(window);
	if (it != s_perWindowDataMap->end())
		return it->second->SwapChain;

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::WaitIdle()
{
	m_device->WaitIdle();
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
		extensions.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
		s_raytracingExtension = true;
	}

	if(physicalDevice->IsExtensionSupported(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME);
		s_samplerYcbcrConversionExtension = true;
	}

	return extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::AddDefaultResources()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPrefix, "Creating DefaultResources");
#endif
	
	s_NullDescriptors = TRAP::MakeScope<struct NullDescriptors>();

	//1D Texture
	TextureDesc textureDesc{};
	textureDesc.ArraySize = 1;
	textureDesc.Depth = 1;
	textureDesc.Format = ImageFormat::R8G8B8A8_UNORM;
	textureDesc.Height = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleCount = SampleCount::SampleCount1;
	textureDesc.StartState = ResourceState::Common;
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.Width = 1;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1D)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1D)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);

	//1D Texture Array
	textureDesc.ArraySize = 2;
	textureDesc.Descriptors = DescriptorType::Texture;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1DArray)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1DArray)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);

	//2D Texture
	textureDesc.Width = 2;
	textureDesc.Height = 2;
	textureDesc.ArraySize = 1;
	textureDesc.Descriptors = DescriptorType::Texture;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2D)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2D)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);

	//2D MS Texture
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.SampleCount = SampleCount::SampleCount2;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DMS)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);
	textureDesc.SampleCount = SampleCount::SampleCount1;

	//2D Texture Array
	textureDesc.ArraySize = 2;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DArray)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DArray)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);

	//2D MS Texture Array
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.SampleCount = SampleCount::SampleCount2;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DMSArray)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);
	textureDesc.SampleCount = SampleCount::SampleCount1;

	//3D Texture
	textureDesc.Depth = 2;
	textureDesc.ArraySize = 1;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim3D)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim3D)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);

	//Cube Texture
	textureDesc.Depth = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Descriptors = DescriptorType::TextureCube;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCube)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);
	textureDesc.ArraySize = 6 * 2;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCubeArray)] = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, m_vma);

	BufferDesc bufferDesc{};
	bufferDesc.Descriptors = DescriptorType::Buffer | DescriptorType::UniformBuffer;
	bufferDesc.MemoryUsage = ResourceMemoryUsage::GPUOnly;
	bufferDesc.StartState = ResourceState::Common;
	bufferDesc.Size = sizeof(uint32_t);
	bufferDesc.FirstElement = 0;
	bufferDesc.ElementCount = 1;
	bufferDesc.StructStride = sizeof(uint32_t);
	bufferDesc.Format = ImageFormat::R32_UINT;
	s_NullDescriptors->DefaultBufferSRV = TRAP::MakeRef<VulkanBuffer>(bufferDesc);
	bufferDesc.Descriptors = DescriptorType::RWBuffer;
	s_NullDescriptors->DefaultBufferUAV = TRAP::MakeRef<VulkanBuffer>(bufferDesc);

	SamplerDesc samplerDesc{};
	samplerDesc.AddressU = AddressMode::ClampToBorder;
	samplerDesc.AddressV = AddressMode::ClampToBorder;
	samplerDesc.AddressW = AddressMode::ClampToBorder;
	s_NullDescriptors->DefaultSampler = TRAP::MakeRef<VulkanSampler>(samplerDesc);

	BlendStateDesc blendStateDesc{};
	blendStateDesc.DstAlphaFactors[0] = BlendConstant::Zero;
	blendStateDesc.DstFactors[0] = BlendConstant::Zero;
	blendStateDesc.SrcAlphaFactors[0] = BlendConstant::One;
	blendStateDesc.SrcFactors[0] = BlendConstant::One;
	blendStateDesc.Masks[0] = (0x1 | 0x2 | 0x4 | 0x8);
	blendStateDesc.RenderTargetMask = BlendStateTargets::BlendStateTargetAll;
	blendStateDesc.IndependentBlend = false;
	DefaultBlendDesc = UtilToBlendDesc(blendStateDesc, DefaultBlendAttachments);

	DepthStateDesc depthStateDesc{};
	depthStateDesc.DepthFunc = CompareMode::LessOrEqual;
	depthStateDesc.DepthTest = false;
	depthStateDesc.DepthWrite = false;
	depthStateDesc.StencilBackFunc = CompareMode::Always;
	depthStateDesc.StencilFrontFunc = CompareMode::Always;
	depthStateDesc.StencilReadMask = 0xFF;
	depthStateDesc.StencilWriteMask = 0xFF;
	DefaultDepthDesc = UtilToDepthDesc(depthStateDesc);

	RasterizerStateDesc rasterizerStateDesc{};
	rasterizerStateDesc.CullMode = CullMode::Back;
	DefaultRasterizerDesc = UtilToRasterizerDesc(rasterizerStateDesc);

	//Create Command Buffer to transition resources to the correct state
	QueueDesc queueDesc{};
	queueDesc.Type = QueueType::Graphics;
	TRAP::Ref<VulkanQueue> graphicsQueue = TRAP::MakeRef<VulkanQueue>(queueDesc);

	CommandPoolDesc cmdPoolDesc{};
	cmdPoolDesc.Queue = graphicsQueue;
	cmdPoolDesc.Transient = true;
	TRAP::Ref<VulkanCommandPool> cmdPool = TRAP::MakeRef<VulkanCommandPool>(cmdPoolDesc);
	
	CommandBuffer* cmd = cmdPool->AllocateCommandBuffer(false);

	//Transition resources
	cmd->Begin();

	std::vector<BufferBarrier> bufferBarriers;
	std::vector<TextureBarrier> textureBarriers;

	for (uint32_t dim = 0; dim < static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount); ++dim)
	{
		if (s_NullDescriptors->DefaultTextureSRV[dim])
			textureBarriers.push_back({ s_NullDescriptors->DefaultTextureSRV[dim], ResourceState::Undefined, ResourceState::ShaderResource });

		if (s_NullDescriptors->DefaultTextureUAV[dim])
			textureBarriers.push_back({s_NullDescriptors->DefaultTextureUAV[dim], ResourceState::Undefined, ResourceState::UnorderedAccess});
	}

	bufferBarriers.push_back({ s_NullDescriptors->DefaultBufferSRV, ResourceState::Undefined, ResourceState::ShaderResource });
	bufferBarriers.push_back({ s_NullDescriptors->DefaultBufferUAV, ResourceState::Undefined, ResourceState::UnorderedAccess });

	cmd->ResourceBarrier(bufferBarriers, textureBarriers, {});
	cmd->End();

	QueueSubmitDesc submitDesc{};
	submitDesc.Cmds.push_back(cmd);
	graphicsQueue->Submit(submitDesc);
	graphicsQueue->WaitQueueIdle();

	//Delete Command Buffer
	cmdPool->FreeCommandBuffer(cmd);
	cmdPool.reset();
	graphicsQueue.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::RemoveDefaultResources()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPrefix, "Destroying DefaultResources");
#endif
	
	for(uint32_t dim = 0; dim < static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount); ++dim)
	{
		if (s_NullDescriptors->DefaultTextureSRV[dim])
			s_NullDescriptors->DefaultTextureSRV[dim].reset();

		if (s_NullDescriptors->DefaultTextureUAV[dim])
			s_NullDescriptors->DefaultTextureUAV[dim].reset();
	}

	s_NullDescriptors->DefaultBufferSRV.reset();
	s_NullDescriptors->DefaultBufferUAV.reset();

	s_NullDescriptors->DefaultSampler.reset();
	
	s_NullDescriptors.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::RenderPassMap& TRAP::Graphics::API::VulkanRenderer::GetRenderPassMap()
{
	//Only need a lock when creating a new RenderPass Map for this thread
	std::lock_guard<std::mutex> lock(s_renderPassMutex);
	auto it = s_renderPassMap->find(std::this_thread::get_id());
	if (it == s_renderPassMap->end())
	{
		(*s_renderPassMap)[std::this_thread::get_id()] = {};
		return (*s_renderPassMap)[std::this_thread::get_id()];
	}

	return it->second;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::FrameBufferMap& TRAP::Graphics::API::VulkanRenderer::GetFrameBufferMap()
{
	//Only need a lock when creating a new FrameBuffer Map for this thread
	std::lock_guard<std::mutex> lock(s_renderPassMutex);
	auto it = s_frameBufferMap->find(std::this_thread::get_id());
	if(it == s_frameBufferMap->end())
	{
		(*s_frameBufferMap)[std::this_thread::get_id()] = {};
		return (*s_frameBufferMap)[std::this_thread::get_id()];
	}

	return it->second;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::API::VulkanInstance> TRAP::Graphics::API::VulkanRenderer::GetInstance() const
{
	return m_instance;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::API::VulkanDevice> TRAP::Graphics::API::VulkanRenderer::GetDevice() const
{
	return m_device;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::API::VulkanMemoryAllocator> TRAP::Graphics::API::VulkanRenderer::GetVMA() const
{
	return m_vma;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::API::VulkanDescriptorPool> TRAP::Graphics::API::VulkanRenderer::GetDescriptorPool() const
{
	return m_descriptorPool;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Ref<TRAP::Graphics::Pipeline>& TRAP::Graphics::API::VulkanRenderer::GetPipeline(const PipelineDesc& desc)
{
	std::size_t hash = std::hash<PipelineDesc>{}(desc);
	const auto it = s_pipelines.find(hash);

	if(it == s_pipelines.end())
	{
		std::lock_guard<std::mutex> lock(s_pipelineMutex);
		TP_TRACE(Log::RendererVulkanPipelinePrefix, "Recreating Graphic Pipeline...");
		//Lock while inserting new Pipeline
		s_pipelines.insert({ hash, Pipeline::Create(desc) });
		TP_TRACE(Log::RendererVulkanPipelinePrefix, "Cached Graphic Pipeline");
		return s_pipelines[hash];
	}
	
	return it->second;
}