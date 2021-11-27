#include "TRAPPCH.h"
#include "VulkanRenderer.h"

#include "Application.h"
#include "Window/Window.h"
#include "Window/WindowingAPI.h"
#include "Utils/Utils.h"
#include "Utils/Dialogs/Dialogs.h"
#include "FS/FS.h"
#include "VulkanCommon.h"

#include "Objects/VulkanSemaphore.h"
#include "Objects/VulkanFence.h"
#include "Objects/VulkanSampler.h"
#include "Objects/VulkanBuffer.h"
#include "Objects/VulkanCommandPool.h"
#include "Objects/VulkanCommandBuffer.h"
#include "Objects/VulkanDescriptorPool.h"
#include "Objects/VulkanMemoryAllocator.h"
#include "Objects/VulkanDevice.h"
#include "Objects/VulkanPhysicalDevice.h"
#include "Objects/VulkanInstance.h"
#include "Objects/VulkanDebug.h"
#include "Objects/VulkanRootSignature.h"

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/PipelineDescHash.h"
#include "Graphics/API/Objects/Pipeline.h"
#include "Graphics/API/Objects/RenderTarget.h"
#include "Graphics/API/Objects/RootSignature.h"
#include "Graphics/API/Objects/PipelineCache.h"
#include "Graphics/API/Objects/Pipeline.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Textures/TextureBase.h"
#include "Utils/Dialogs/Dialogs.h"

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
VkPipelineRasterizationStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultRasterizerDesc = UtilToRasterizerDesc
(
	{CullMode::Back, {}, {}, {}, {}, {}}
);
VkPipelineDepthStencilStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultDepthDesc = UtilToDepthDesc
(
	{ false, false, CompareMode::Less, {}, 0xFF, 0xFF, CompareMode::Always, {}, {}, {}, CompareMode::Always,
	  {}, {}, {} }
);
VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultBlendDesc = UtilToBlendDesc
(
	{ {BlendConstant::One}, {BlendConstant::Zero}, {BlendConstant::One}, {BlendConstant::Zero}, {}, {},
	  {(BIT(0) | BIT(1) | BIT(2) | BIT(3)) }, BlendStateTargets::BlendStateTargetAll, false
	}, DefaultBlendAttachments
);

std::unordered_map<std::thread::id, TRAP::Graphics::API::VulkanRenderer::RenderPassMap> TRAP::Graphics::API::VulkanRenderer::s_renderPassMap = {};
std::unordered_map<std::thread::id, TRAP::Graphics::API::VulkanRenderer::FrameBufferMap> TRAP::Graphics::API::VulkanRenderer::s_frameBufferMap = {};
std::mutex TRAP::Graphics::API::VulkanRenderer::s_renderPassMutex{};

std::unordered_map<uint64_t, TRAP::Ref<TRAP::Graphics::Pipeline>> TRAP::Graphics::API::VulkanRenderer::s_pipelines{};
std::unordered_map<uint64_t,
                   TRAP::Ref<TRAP::Graphics::PipelineCache>> TRAP::Graphics::API::VulkanRenderer::s_pipelineCaches{};
std::mutex TRAP::Graphics::API::VulkanRenderer::s_pipelineMutex{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::VulkanRenderer()
	: m_instance(nullptr),
	  m_debug(nullptr),
	  m_device(nullptr),
	  m_vma(nullptr)
{
	s_renderer = this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::~VulkanRenderer()
{
	TP_DEBUG(Log::RendererVulkanPrefix, "Destroying Renderer");

	s_descriptorPool.reset();

	s_ResourceLoader.reset();

	RemoveDefaultResources();

	std::lock_guard<std::mutex> lock(s_renderPassMutex);
	s_frameBufferMap.clear();

	s_renderPassMap.clear();

	s_pipelines.clear();

	for(auto& [hash, cache] : s_pipelineCaches)
	{
		if(!cache)
			continue;

		cache->Save(TRAP::FS::GetGameTempFolderPath() / (std::to_string(hash) + ".cache"));
		cache.reset();
	}
	s_pipelineCaches.clear();

	//Free everything in order
	//Should happen automagically through Scope destructors
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::StartGraphicRecording(const TRAP::Scope<PerWindowData>& p)
{
	TRAP_ASSERT(p);

	if(p->Window->IsMinimized () || p->Recording)
		return;

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
	loadActions.ClearDepth = p->ClearDepth;
	loadActions.ClearStencil = p->ClearStencil;
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({ renderTarget }, nullptr, &loadActions, {}, {},
	                                                           std::numeric_limits<uint32_t>::max(),
															   std::numeric_limits<uint32_t>::max());

	//Set Default Dynamic Viewport & Scissor
	p->GraphicCommandBuffers[p->ImageIndex]->SetViewport(0.0f, 0.0f, static_cast<float>(p->Window->GetWidth()),
	                                                     static_cast<float>(p->Window->GetHeight()), 0.0f, 1.0f);
	p->GraphicCommandBuffers[p->ImageIndex]->SetScissor(0, 0, p->Window->GetWidth(), p->Window->GetHeight());
	if(p->CurrentGraphicsPipeline)
		p->GraphicCommandBuffers[p->ImageIndex]->BindPipeline(p->CurrentGraphicsPipeline);

	p->Recording = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::EndGraphicRecording(const TRAP::Scope<PerWindowData>& p)
{
	if(p->Window->IsMinimized() || !p->Recording)
		return;

	//End Recording
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({}, nullptr, nullptr, {}, {},
	                                                           std::numeric_limits<uint32_t>::max(),
															   std::numeric_limits<uint32_t>::max());

	const RenderTargetBarrier barrier = { p->SwapChain->GetRenderTargets()[p->CurrentSwapChainImageIndex],
	                                      ResourceState::RenderTarget, ResourceState::Present };
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier({}, {}, { barrier });

	p->GraphicCommandBuffers[p->ImageIndex]->End();

	QueueSubmitDesc submitDesc{};
	submitDesc.Cmds = { p->GraphicCommandBuffers[p->ImageIndex] };
	submitDesc.SignalSemaphores = { p->RenderCompleteSemaphores[p->ImageIndex] };
	submitDesc.WaitSemaphores = { p->ImageAcquiredSemaphore };
	submitDesc.SignalFence = p->RenderCompleteFences[p->ImageIndex];
	s_graphicQueue->Submit(submitDesc);

	QueuePresentDesc presentDesc{};
	presentDesc.Index = static_cast<uint8_t>(p->CurrentSwapChainImageIndex);
	presentDesc.WaitSemaphores = { p->RenderCompleteSemaphores[p->ImageIndex] };
	presentDesc.SwapChain = p->SwapChain;
	const PresentStatus presentStatus = s_graphicQueue->Present(presentDesc);

	if (presentStatus == PresentStatus::DeviceReset || presentStatus == PresentStatus::Failed)
	{
		if(presentStatus == PresentStatus::DeviceReset)
			TRAP::Utils::Dialogs::ShowMsgBox("Presenting failed", "Vulkan: Device was reset while presenting!\n"
											 "Error code: 0x000D",
			                                 TRAP::Utils::Dialogs::Style::Error,
											 TRAP::Utils::Dialogs::Buttons::Quit);
		else if(presentStatus == PresentStatus::Failed)
		{
			TRAP::Utils::Dialogs::ShowMsgBox("Presenting failed", "Vulkan: Presenting failed!\n"
											 "Error code: 0x000F",
			                                 TRAP::Utils::Dialogs::Style::Error,
											 TRAP::Utils::Dialogs::Buttons::Quit);
		}

		TRAP::Application::Shutdown();
	}
	else if (presentStatus == PresentStatus::OutOfDate)
	{
		//Recreation needed
		//Clear SwapChain reference
		presentDesc = {};

		p->SwapChain.reset();

		SwapChainDesc swapChainDesc{};
		swapChainDesc.Window = p->Window;
		swapChainDesc.PresentQueues = { s_graphicQueue };
		swapChainDesc.Width = p->Window->GetWidth();
		swapChainDesc.Height = p->Window->GetHeight();
		swapChainDesc.ImageCount = RendererAPI::ImageCount;
		swapChainDesc.ColorFormat = SwapChain::GetRecommendedSwapchainFormat(true);
		swapChainDesc.ClearColor = p->ClearColor;
		swapChainDesc.ClearDepth = p->ClearDepth;
		swapChainDesc.ClearStencil = p->ClearStencil;
		swapChainDesc.EnableVSync = p->CurrentVSync;
		p->SwapChain = SwapChain::Create(swapChainDesc);

		if (!p->SwapChain)
		{
			TRAP::Utils::Dialogs::ShowMsgBox("Swapchain creation failed", "Vulkan: Unable to create swapchain!\n"
											 "Error code: 0x0010",
			                                 TRAP::Utils::Dialogs::Style::Error,
											 TRAP::Utils::Dialogs::Buttons::Quit);
			TRAP::Application::Shutdown();
		}
	}

	p->ImageIndex = (p->ImageIndex + 1) % RendererAPI::ImageCount;

	p->Recording = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::InitInternal(const std::string_view gameName)
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
			if(UUID != devUUID)
				continue;

			physicalDevice = TRAP::MakeScope<VulkanPhysicalDevice>(m_instance, devUUID);
			break;
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

	s_descriptorPool = TRAP::MakeRef<VulkanDescriptorPool>(8192);

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
	TP_INFO(Log::RendererVulkanPrefix, "Driver Vulkan Version: ", VK_VERSION_MAJOR(devProps.apiVersion), '.',
	        VK_VERSION_MINOR(devProps.apiVersion), '.', VK_VERSION_PATCH(devProps.apiVersion));
	TP_INFO(Log::RendererVulkanPrefix, "Renderer: ", devProps.deviceName);
	TP_INFO(Log::RendererVulkanPrefix, "Driver: ", VK_VERSION_MAJOR(devProps.driverVersion), '.',
	        VK_VERSION_MINOR(devProps.driverVersion), '.', VK_VERSION_PATCH(devProps.driverVersion));
	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");

	m_rendererTitle = "[Vulkan " + std::to_string(VK_VERSION_MAJOR(devProps.apiVersion)) + "." +
							       std::to_string(VK_VERSION_MINOR(devProps.apiVersion)) + "." +
								   std::to_string(VK_VERSION_PATCH(devProps.apiVersion)) + "]";
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Present(Window* window)
{
	const TRAP::Scope<PerWindowData>& p = s_perWindowDataMap[window];

	EndGraphicRecording(p);
	if (p->CurrentVSync != p->NewVSync) //Change V-Sync state only between frames!
	{
		//std::lock_guard<std::mutex> lock(s_perWindowDataMutex);
		if(p->SwapChain)
			p->SwapChain->ToggleVSync();
		p->CurrentVSync = p->NewVSync;
	}
	StartGraphicRecording(p);
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetVSync(const bool vsync, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	s_perWindowDataMap[window]->NewVSync = vsync;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetClearColor(const Math::Vec4& color, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	s_perWindowDataMap[window]->ClearColor = color;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetClearDepth(const float depth, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	s_perWindowDataMap[window]->ClearDepth = depth;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetClearStencil(const uint32_t stencil, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	s_perWindowDataMap[window]->ClearStencil = stencil;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthTesting(const bool enabled, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthTest = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthWriting(const bool enabled, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthWrite = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthFunction(const CompareMode function, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthFunc = function;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthFail(const StencilOp front, const StencilOp back, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	);

	gpd.DepthState->DepthFrontFail = front;
	gpd.DepthState->DepthBackFail = back;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthBias(const int32_t depthBias, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).RasterizerState->DepthBias = depthBias;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthBiasSlopeFactor(const float factor, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).RasterizerState->SlopeScaledDepthBias = factor;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilTesting(const bool enabled, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).DepthState->StencilTest = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilFail(const StencilOp front, const StencilOp back,
                                                         Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	);

	gpd.DepthState->StencilFrontFail = front;
	gpd.DepthState->StencilBackFail = back;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilPass(const StencilOp front, const StencilOp back, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	);

	gpd.DepthState->StencilFrontPass = front;
	gpd.DepthState->StencilBackPass = back;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilFunction(const CompareMode front, const CompareMode back,
                                                             Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	);

	gpd.DepthState->StencilFrontFunc = front;
	gpd.DepthState->StencilBackFunc = back;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilMask(const uint8_t read, const uint8_t write, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	);

	gpd.DepthState->StencilReadMask = read;
	gpd.DepthState->StencilWriteMask = write;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetCullMode(const CullMode mode, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).RasterizerState->CullMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetFillMode(const FillMode mode, Window* window)
{
	if(mode != FillMode::Solid && !GPUSettings.FillModeNonSolid)
		return;

	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& p = s_perWindowDataMap[window];

	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RasterizerState->FillMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetPrimitiveTopology(const PrimitiveTopology topology, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& p = s_perWindowDataMap[window];

	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).PrimitiveTopology = topology;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetFrontFace(const FrontFace face, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).RasterizerState->FrontFace = face;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha,
                                                       Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	);

	gpd.BlendState->BlendModes = { modeRGB };
	gpd.BlendState->BlendAlphaModes = { modeAlpha };
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendConstant(const BlendConstant sourceRGB,
														   const BlendConstant sourceAlpha,
                                                           const BlendConstant destinationRGB,
                                                           const BlendConstant destinationAlpha, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	);

	gpd.BlendState->SrcFactors = {sourceRGB};
	gpd.BlendState->DstFactors = {destinationRGB};
	gpd.BlendState->SrcAlphaFactors = {sourceAlpha};
	gpd.BlendState->DstAlphaFactors = {destinationAlpha};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Clear(const ClearBufferType clearType, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& data = s_perWindowDataMap[window];
	const TRAP::Ref<RenderTarget>& renderTarget = data->SwapChain->GetRenderTargets()[data->ImageIndex];

	if(static_cast<uint32_t>(clearType & ClearBufferType::Color) != 0)
	{
		data->GraphicCommandBuffers[data->ImageIndex]->Clear(data->ClearColor, renderTarget->GetWidth(),
		                                                     renderTarget->GetHeight());
	}

	if(static_cast<uint32_t>(clearType & ClearBufferType::Depth_Stencil) != 0 &&
	   TRAP::Graphics::API::ImageFormatIsDepthAndStencil(renderTarget->GetImageFormat()))
	{
		data->GraphicCommandBuffers[data->ImageIndex]->Clear(data->ClearDepth, data->ClearStencil,
															 renderTarget->GetWidth(), renderTarget->GetHeight());
	}
	else if(static_cast<uint32_t>(clearType & ClearBufferType::Depth) != 0 &&
	        (TRAP::Graphics::API::ImageFormatIsDepthAndStencil(renderTarget->GetImageFormat()) ||
			 TRAP::Graphics::API::ImageFormatIsDepthOnly(renderTarget->GetImageFormat())))
	{
		data->GraphicCommandBuffers[data->ImageIndex]->Clear(data->ClearDepth, renderTarget->GetWidth(),
															 renderTarget->GetHeight());
	}
	else if(static_cast<uint32_t>(clearType & ClearBufferType::Stencil) != 0 &&
	        TRAP::Graphics::API::ImageFormatHasStencil(renderTarget->GetImageFormat()))
	{
		data->GraphicCommandBuffers[data->ImageIndex]->Clear(data->ClearStencil, renderTarget->GetWidth(),
		 													 renderTarget->GetHeight());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width,
                                                      const uint32_t height, const float minDepth,
                                                      const float maxDepth, Window* window)
{
	if (width == 0 || height == 0)
		return;

	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& data = s_perWindowDataMap[window];

	data->GraphicCommandBuffers[data->ImageIndex]->SetViewport(static_cast<float>(x), static_cast<float>(y),
	                                                           static_cast<float>(width), static_cast<float>(height),
	                                                           minDepth, maxDepth);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width,
                                                     const uint32_t height, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& data = s_perWindowDataMap[window];

	data->GraphicCommandBuffers[data->ImageIndex]->SetScissor(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Draw(const uint32_t vertexCount, const uint32_t firstVertex,
                                               Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& data = s_perWindowDataMap[window];

	data->GraphicCommandBuffers[data->ImageIndex]->Draw(vertexCount, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::DrawIndexed(const uint32_t indexCount, const uint32_t firstIndex,
                                                      const uint32_t firstVertex, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& data = s_perWindowDataMap[window];

	data->GraphicCommandBuffers[data->ImageIndex]->DrawIndexed(indexCount, firstIndex, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::DrawInstanced(const uint32_t vertexCount, const uint32_t instanceCount,
                                                        const uint32_t firstVertex, const uint32_t firstInstance,
                                                        Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& data = s_perWindowDataMap[window];

	data->GraphicCommandBuffers[data->ImageIndex]->DrawInstanced(vertexCount, firstVertex, instanceCount,
	                                                             firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::DrawIndexedInstanced(const uint32_t indexCount, const uint32_t instanceCount,
                                                               const uint32_t firstIndex, const uint32_t firstInstance,
						                                       const uint32_t firstVertex, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& data = s_perWindowDataMap[window];

	data->GraphicCommandBuffers[data->ImageIndex]->DrawIndexedInstanced(indexCount, firstIndex, instanceCount,
	                                                                    firstInstance, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindShader(Shader* shader, Window* window) const
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& data = s_perWindowDataMap[window];

	const ShaderStage stages = shader->GetShaderStages();

	if (stages == ShaderStage::RayTracing)
	{
		//TODO RayTracingPipelineDesc.Pipeline.ShaderProgram = shader;
		//Bind pipeline
	}
	else if (stages == ShaderStage::Compute)
	{
		//TODO ComputePipelineDesc.Pipeline.ShaderProgram = shader;
		//Bind pipeline
	}
	else if (stages != ShaderStage::None && stages != ShaderStage::SHADER_STAGE_COUNT)
	{
		GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(data->GraphicsPipelineDesc.Pipeline);

		if(gpd.ShaderProgram == shader) //TODO Shouldn't it just return?!
		{
			data->CurrentGraphicsPipeline = GetPipeline(data->GraphicsPipelineDesc);
			data->GraphicCommandBuffers[data->ImageIndex]->BindPipeline(data->CurrentGraphicsPipeline);

			//Bind Descriptors
			for(uint32_t i = 0; i < RendererAPI::MaxDescriptorSets; ++i)
			{
				if(shader->GetDescriptorSets()[i])
				{
					data->GraphicCommandBuffers[data->ImageIndex]->BindDescriptorSet(i == 0 ? 0 : data->ImageIndex,
				                                                                     *(shader->GetDescriptorSets()[i]));
				}
			}

			return;
		}

		gpd.ShaderProgram = shader;
		gpd.RootSignature = shader->GetRootSignature();

		data->CurrentGraphicsPipeline = GetPipeline(data->GraphicsPipelineDesc);
		data->GraphicCommandBuffers[data->ImageIndex]->BindPipeline(data->CurrentGraphicsPipeline);

		//Bind Descriptors
		for(uint32_t i = 0; i < RendererAPI::MaxDescriptorSets; ++i)
		{
			if(shader->GetDescriptorSets()[i])
			{
				data->GraphicCommandBuffers[data->ImageIndex]->BindDescriptorSet(i == 0 ? 0 : data->ImageIndex,
																			 	 *(shader->GetDescriptorSets()[i]));
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer,
                                                           const VertexBufferLayout& layout, Window* window)
{
	//Vertex Buffer realistically only consists of floating point values
	constexpr auto ShaderDataTypeToImageFormat = [](const ShaderDataType s) -> ImageFormat
	{
		switch(s)
		{
		case ShaderDataType::Float:
			return ImageFormat::R32_SFLOAT;

		case ShaderDataType::Float2:
			return ImageFormat::R32G32_SFLOAT;

		case ShaderDataType::Float3:
			return ImageFormat::R32G32B32_SFLOAT;

		case ShaderDataType::Float4:
			return ImageFormat::R32G32B32A32_SFLOAT;

		case ShaderDataType::None:
		default:
			return ImageFormat::Undefined;
		}
	};

	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& p = s_perWindowDataMap[window];

	p->GraphicCommandBuffers[p->ImageIndex]->BindVertexBuffer({ vBuffer }, { layout.GetStride() }, {});

	TRAP::Ref<VertexLayout> lay = TRAP::MakeRef<VertexLayout>();
	const std::vector<VertexBufferElement>& elements = layout.GetElements();
	lay->AttributeCount = static_cast<uint32_t>(elements.size());
	for(uint32_t i = 0; i < elements.size(); ++i)
	{
		lay->Attributes[i].Binding = 0;
		lay->Attributes[i].Location = i;
		lay->Attributes[i].Format = ShaderDataTypeToImageFormat(elements[i].Type);
		lay->Attributes[i].Rate = VertexAttributeRate::Vertex;
		lay->Attributes[i].Offset = elements[i].Offset;
	}
	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).VertexLayout = lay;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer,
                                                          const IndexType indexType, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& p = s_perWindowDataMap[window];

	p->GraphicCommandBuffers[p->ImageIndex]->BindIndexBuffer(iBuffer, indexType, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindDescriptorSet(DescriptorSet& dSet, const uint32_t index,
                                                            Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& p = s_perWindowDataMap[window];

	p->GraphicCommandBuffers[p->ImageIndex]->BindDescriptorSet(index, dSet);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindPushConstants(const char* name, const void* constantsData,
                                                            Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& p = s_perWindowDataMap[window];

	p->GraphicCommandBuffers[p->ImageIndex]->BindPushConstants
	(
		std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RootSignature,
		name, constantsData
	);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindPushConstantsByIndex(const uint32_t paramIndex,
                                                                   const void* constantsData, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	const TRAP::Scope<PerWindowData>& p = s_perWindowDataMap[window];

	p->GraphicCommandBuffers[p->ImageIndex]->BindPushConstantsByIndex
	(
		std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RootSignature,
		paramIndex, constantsData
	);
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

	return s_perWindowDataMap[window]->CurrentVSync;
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
	if(!s_usableGPUs.empty())
		return s_usableGPUs;

	for (const auto& [score, devUUID] : VulkanPhysicalDevice::GetAllRatedPhysicalDevices(m_instance))
	{
		VkPhysicalDevice dev = VulkanPhysicalDevice::FindPhysicalDeviceViaUUID(m_instance, devUUID);
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(dev, &props);

		s_usableGPUs.emplace_back(props.deviceName, devUUID);
	}

	return s_usableGPUs;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::InitPerWindowData(Window* window)
{
	std::lock_guard<std::mutex> lock(s_perWindowDataMutex);

	if (s_perWindowDataMap.find(window) != s_perWindowDataMap.end())
		//Window is already in map
		return;

	//Add new Window to map
	TRAP::Scope<PerWindowData> p = TRAP::MakeScope<PerWindowData>();

	p->Window = window;

	//For each buffered image
	for (uint32_t i = 0; i < RendererAPI::ImageCount; ++i)
	{
		//Create Graphic Command Pool
		p->GraphicCommandPools[i] = CommandPool::Create({ s_graphicQueue, false });
		//Allocate Graphic Command Buffer
		p->GraphicCommandBuffers[i] = p->GraphicCommandPools[i]->AllocateCommandBuffer(false);

		//Create Render Fences/Semaphores
		p->RenderCompleteFences[i] = Fence::Create();
		p->RenderCompleteSemaphores[i] = Semaphore::Create();
	}

	//Image Acquire Semaphore
	p->ImageAcquiredSemaphore = Semaphore::Create();

	if (!p->Window->IsMinimized())
	{
		//Create Swapchain
		p->CurrentVSync = p->NewVSync = window->GetVSync();
		SwapChainDesc swapChainDesc{};
		swapChainDesc.Window = window;
		swapChainDesc.PresentQueues = { s_graphicQueue };
		swapChainDesc.Width = window->GetWidth();
		swapChainDesc.Height = window->GetHeight();
		swapChainDesc.ImageCount = RendererAPI::ImageCount;
		swapChainDesc.ColorFormat = SwapChain::GetRecommendedSwapchainFormat(true);
		swapChainDesc.EnableVSync = p->CurrentVSync;
		swapChainDesc.ClearColor = p->ClearColor;
		swapChainDesc.ClearDepth = p->ClearDepth;
		swapChainDesc.ClearStencil = p->ClearStencil;
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
	gpd.RasterizerState->FrontFace = FrontFace::CounterClockwise;
	gpd.RasterizerState->DepthClampEnable = false;

	gpd.BlendState = TRAP::MakeRef<BlendStateDesc>();
	gpd.BlendState->SrcFactors = {BlendConstant::One };
	gpd.BlendState->DstFactors = {BlendConstant::Zero };
	gpd.BlendState->SrcAlphaFactors = { BlendConstant::One };
	gpd.BlendState->DstAlphaFactors = { BlendConstant::Zero};
	gpd.BlendState->BlendModes = {};
	gpd.BlendState->BlendAlphaModes = {};
	gpd.BlendState->Masks = {(BIT(0) | BIT(1) | BIT(2) | BIT(3))};
	gpd.BlendState->RenderTargetMask = BlendStateTargets::BlendStateTargetAll;
	gpd.BlendState->IndependentBlend = false;

	gpd.VertexLayout = TRAP::MakeRef<VertexLayout>();
	const TRAP::Ref<VertexLayout>& vLayout = gpd.VertexLayout;
	vLayout->AttributeCount = 1;
	vLayout->Attributes[0].Format = ImageFormat::R32G32B32_SFLOAT;
	vLayout->Attributes[0].Binding = 0;
	vLayout->Attributes[0].Location = 0;
	vLayout->Attributes[0].Offset = 0;

	s_perWindowDataMap[window] = std::move(p);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::RemovePerWindowData(Window* window)
{
	std::lock_guard<std::mutex> lock(s_perWindowDataMutex);
	if (s_perWindowDataMap.find(window) != s_perWindowDataMap.end())
		s_perWindowDataMap.erase(window);
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
		Utils::Dialogs::ShowMsgBox("Vulkan API error", "Mandatory Vulkan surface extensions are unsupported!\n"
								   "Error code: 0x0003", Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererVulkanPrefix, "Mandatory Vulkan surface extensions are unsupported!");
		TRAP::Application::Shutdown();
		exit(-1);
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
		Utils::Dialogs::ShowMsgBox("Vulkan API error", "Mandatory Vulkan swapchain extension is unsupported!\n"
								   "Error code: 0x0004", Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererVulkanPrefix, "Mandatory Vulkan swapchain extension is unsupported!");
		TRAP::Application::Shutdown();
		exit(-1);
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

	if(physicalDevice->IsExtensionSupported(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME))
		extensions.emplace_back(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME);

	return extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::AddDefaultResources() const
{
#ifdef VERBOSE_GRAPHICS_DEBUG
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
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1D)] = TRAP::Graphics::TextureBase::Create(textureDesc);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1D)] = TRAP::Graphics::TextureBase::Create(textureDesc);

	//1D Texture Array
	textureDesc.ArraySize = 2;
	textureDesc.Descriptors = DescriptorType::Texture;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1DArray)] = TRAP::Graphics::TextureBase::Create(textureDesc);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1DArray)] = TRAP::Graphics::TextureBase::Create(textureDesc);

	//2D Texture
	textureDesc.Width = 2;
	textureDesc.Height = 2;
	textureDesc.ArraySize = 1;
	textureDesc.Descriptors = DescriptorType::Texture;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2D)] = TRAP::Graphics::TextureBase::Create(textureDesc);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2D)] = TRAP::Graphics::TextureBase::Create(textureDesc);

	//2D MS Texture
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.SampleCount = SampleCount::SampleCount2;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DMS)] = TRAP::Graphics::TextureBase::Create(textureDesc);
	textureDesc.SampleCount = SampleCount::SampleCount1;

	//2D Texture Array
	textureDesc.ArraySize = 2;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DArray)] = TRAP::Graphics::TextureBase::Create(textureDesc);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DArray)] = TRAP::Graphics::TextureBase::Create(textureDesc);

	//2D MS Texture Array
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.SampleCount = SampleCount::SampleCount2;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DMSArray)] = TRAP::Graphics::TextureBase::Create(textureDesc);
	textureDesc.SampleCount = SampleCount::SampleCount1;

	//3D Texture
	textureDesc.Depth = 2;
	textureDesc.ArraySize = 1;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim3D)] = TRAP::Graphics::TextureBase::Create(textureDesc);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim3D)] = TRAP::Graphics::TextureBase::Create(textureDesc);

	//Cube Texture
	textureDesc.Depth = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Descriptors = DescriptorType::TextureCube;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCube)] = TRAP::Graphics::TextureBase::Create(textureDesc);
	textureDesc.ArraySize = 6 * 2;
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCubeArray)] = TRAP::Graphics::TextureBase::Create(textureDesc);

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
	blendStateDesc.Masks[0] = (BIT(0) | BIT(1) | BIT(2) | BIT(3));
	blendStateDesc.RenderTargetMask = BlendStateTargets::BlendStateTargetAll;
	blendStateDesc.IndependentBlend = false;
	DefaultBlendDesc = UtilToBlendDesc(blendStateDesc, DefaultBlendAttachments);

	DepthStateDesc depthStateDesc{};
	depthStateDesc.DepthFunc = CompareMode::Less;
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

	TRAP::Ref<VulkanFence> fence = TRAP::MakeRef<VulkanFence>();

	s_NullDescriptors->InitialTransitionQueue = graphicsQueue;
	s_NullDescriptors->InitialTransitionCmdPool = cmdPool;
	s_NullDescriptors->InitialTransitionCmd = dynamic_cast<VulkanCommandBuffer*>(cmd);
	s_NullDescriptors->InitialTransitionFence = fence;

	//Transition resources
	for (uint32_t dim = 0; dim < static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount); ++dim)
	{
		if (s_NullDescriptors->DefaultTextureSRV[dim])
			UtilInitialTransition(s_NullDescriptors->DefaultTextureSRV[dim], ResourceState::ShaderResource);

		if (s_NullDescriptors->DefaultTextureUAV[dim])
			UtilInitialTransition(s_NullDescriptors->DefaultTextureUAV[dim], ResourceState::UnorderedAccess);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::RemoveDefaultResources()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
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

	s_NullDescriptors->InitialTransitionFence.reset();
	s_NullDescriptors->InitialTransitionCmdPool->FreeCommandBuffer(s_NullDescriptors->InitialTransitionCmd);
	s_NullDescriptors->InitialTransitionCmdPool.reset();
	s_NullDescriptors->InitialTransitionQueue.reset();

	s_NullDescriptors.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::UtilInitialTransition(TRAP::Ref<TRAP::Graphics::TextureBase> texture,
                                                                const RendererAPI::ResourceState startState)
{
	std::lock_guard<std::mutex> lock(s_NullDescriptors->InitialTransitionMutex);
	VulkanCommandBuffer* cmd = s_NullDescriptors->InitialTransitionCmd;
	s_NullDescriptors->InitialTransitionCmdPool->Reset();
	cmd->Begin();
	const TextureBarrier barrier = {std::move(texture), RendererAPI::ResourceState::Undefined, startState};
	cmd->ResourceBarrier({}, {barrier}, {});
	cmd->End();
	RendererAPI::QueueSubmitDesc submitDesc{};
	submitDesc.Cmds = {cmd};
	submitDesc.SignalFence = s_NullDescriptors->InitialTransitionFence;
	s_NullDescriptors->InitialTransitionQueue->Submit(submitDesc);
	s_NullDescriptors->InitialTransitionFence->Wait();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::RenderPassMap& TRAP::Graphics::API::VulkanRenderer::GetRenderPassMap()
{
	//Only need a lock when creating a new RenderPass Map for this thread
	std::lock_guard<std::mutex> lock(s_renderPassMutex);
	auto it = s_renderPassMap.find(std::this_thread::get_id());
	if (it == s_renderPassMap.end())
	{
		s_renderPassMap[std::this_thread::get_id()] = {};
		return s_renderPassMap[std::this_thread::get_id()];
	}

	return it->second;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::FrameBufferMap& TRAP::Graphics::API::VulkanRenderer::GetFrameBufferMap()
{
	//Only need a lock when creating a new FrameBuffer Map for this thread
	std::lock_guard<std::mutex> lock(s_renderPassMutex);
	auto it = s_frameBufferMap.find(std::this_thread::get_id());
	if(it == s_frameBufferMap.end())
	{
		s_frameBufferMap[std::this_thread::get_id()] = {};
		return s_frameBufferMap[std::this_thread::get_id()];
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

const TRAP::Ref<TRAP::Graphics::Pipeline>& TRAP::Graphics::API::VulkanRenderer::GetPipeline(PipelineDesc& desc)
{
	std::size_t hash = std::hash<PipelineDesc>{}(desc);
	const auto pipelineIt = s_pipelines.find(hash);

	if(pipelineIt != s_pipelines.end())
		return pipelineIt->second;

	const auto cacheIt = s_pipelineCaches.find(hash);

	if (cacheIt == s_pipelineCaches.end())
	{
		PipelineCacheLoadDesc cacheDesc{};
		cacheDesc.Path = TRAP::FS::GetGameTempFolderPath() / (std::to_string(hash) + ".cache");
		s_pipelineCaches.insert({ hash, PipelineCache::Create(cacheDesc) });
	}

	desc.Cache = s_pipelineCaches[hash];

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_TRACE(Log::RendererVulkanPipelinePrefix, "Recreating Graphics Pipeline...");
#endif
	{
		TRAP::Ref<TRAP::Graphics::Pipeline> pipeline = Pipeline::Create(desc);
		//Lock while inserting new Pipeline
		std::lock_guard<std::mutex> lock(s_pipelineMutex);
		s_pipelines.insert({ hash, std::move(pipeline) });
	}
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_TRACE(Log::RendererVulkanPipelinePrefix, "Cached Graphics Pipeline");
#endif
	return s_pipelines[hash];
}