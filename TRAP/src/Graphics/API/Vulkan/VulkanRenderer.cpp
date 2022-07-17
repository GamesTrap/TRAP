#include "TRAPPCH.h"
#include "VulkanRenderer.h"

#include "Application.h"
#include "Window/Window.h"
#include "Window/WindowingAPI.h"
#include "Utils/Utils.h"
#include "Utils/Dialogs/Dialogs.h"
#include "FileSystem/FileSystem.h"
#include "VulkanCommon.h"
#include "Embed.h"

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
#include "Objects/VulkanTexture.h"
#include "Objects/VulkanInits.h"
#include "Objects/VulkanShader.h"

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/PipelineDescHash.h"
#include "Graphics/API/Objects/Pipeline.h"
#include "Graphics/API/Objects/RenderTarget.h"
#include "Graphics/API/Objects/RootSignature.h"
#include "Graphics/API/Objects/PipelineCache.h"
#include "Graphics/API/Objects/Pipeline.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/Texture.h"
#include "Utils/Dialogs/Dialogs.h"

TRAP::Graphics::API::VulkanRenderer* TRAP::Graphics::API::VulkanRenderer::s_renderer = nullptr;
//Instance Extensions
bool TRAP::Graphics::API::VulkanRenderer::s_debugUtilsExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_debugReportExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_validationFeaturesExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_swapchainColorSpaceExtension = false;
//Device Extensions
bool TRAP::Graphics::API::VulkanRenderer::s_shaderDrawParameters = false;
bool TRAP::Graphics::API::VulkanRenderer::s_fragmentShaderInterlockExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_drawIndirectCountExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_descriptorIndexingExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_rayTracingExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_samplerYcbcrConversionExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_bufferDeviceAddressExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_memoryBudgetExtension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_maintenance4Extension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_externalMemory = false;
bool TRAP::Graphics::API::VulkanRenderer::s_shadingRate = false;

bool TRAP::Graphics::API::VulkanRenderer::s_debugMarkerSupport = false;

bool TRAP::Graphics::API::VulkanRenderer::s_externalMemoryWin32Extension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_externalFenceWin32Extension = false;
bool TRAP::Graphics::API::VulkanRenderer::s_externalSemaphoreWin32Extension = false;

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

	const auto tempFolder = TRAP::FileSystem::GetGameTempFolderPath();
	if(tempFolder)
	{
		for(auto& [hash, cache] : s_pipelineCaches)
		{
			if(!cache)
				continue;

			cache->Save(*tempFolder / (std::to_string(hash) + ".cache"));
			cache.reset();
		}
	}

	s_pipelineCaches.clear();

	//Free everything in order
	//Should happen automagically through Scope destructors
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::StartGraphicRecording(PerWindowData* const p)
{
	TRAP_ASSERT(p);

	if(p->Recording)
		return;

	//Start Recording
	TRAP::Ref<RenderTarget> renderTarget;
#ifndef TRAP_HEADLESS_MODE
	p->CurrentSwapChainImageIndex = p->SwapChain->AcquireNextImage(p->ImageAcquiredSemaphore, nullptr);
	if(p->AntiAliasing == RendererAPI::AntiAliasing::MSAA)
		renderTarget = p->SwapChain->GetRenderTargetsMSAA()[p->CurrentSwapChainImageIndex];
	else
		renderTarget = p->SwapChain->GetRenderTargets()[p->CurrentSwapChainImageIndex];
#else
	p->CurrentSwapChainImageIndex = (p->CurrentSwapChainImageIndex + 1) % RendererAPI::ImageCount;
	if(p->AntiAliasing == RendererAPI::AntiAliasing::MSAA)
		renderTarget = p->RenderTargetsMSAA[p->CurrentSwapChainImageIndex];
	else
		renderTarget = p->RenderTargets[p->CurrentSwapChainImageIndex];
#endif

	TRAP::Ref<Fence> renderCompleteFence = p->RenderCompleteFences[p->ImageIndex];

	//Stall if CPU is running "Swap Chain Buffer Count" frames ahead of GPU
	if (renderCompleteFence->GetStatus() == FenceStatus::Incomplete)
		renderCompleteFence->Wait();

	//Reset cmd pool for this frame
	p->GraphicCommandPools[p->ImageIndex]->Reset();

	p->GraphicCommandBuffers[p->ImageIndex]->Begin();

#ifndef TRAP_HEADLESS_MODE
	RenderTargetBarrier barrier{renderTarget, ResourceState::Present, ResourceState::RenderTarget};
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#endif

	LoadActionsDesc loadActions{};
	loadActions.LoadActionsColor[0] = LoadActionType::Clear;
	loadActions.ClearColorValues[0] = p->ClearColor;
	loadActions.ClearDepth = p->ClearDepth;
	loadActions.ClearStencil = p->ClearStencil;
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({ renderTarget }, nullptr, &loadActions, nullptr, nullptr,
	                                                           std::numeric_limits<uint32_t>::max(),
															   std::numeric_limits<uint32_t>::max());

	//Set Default Dynamic Viewport & Scissor
	uint32_t width = 0, height = 0;
#ifndef TRAP_HEADLESS_MODE
	width = p->Window->GetWidth();
	height = p->Window->GetHeight();
#else
	width = renderTarget->GetWidth();
	height = renderTarget->GetHeight();
#endif
	p->GraphicCommandBuffers[p->ImageIndex]->SetViewport(0.0f, 0.0f, static_cast<float>(width),
														 static_cast<float>(height), 0.0f, 1.0f);
	p->GraphicCommandBuffers[p->ImageIndex]->SetScissor(0, 0, width, height);
	if(p->CurrentGraphicsPipeline)
		p->GraphicCommandBuffers[p->ImageIndex]->BindPipeline(p->CurrentGraphicsPipeline);

	p->Recording = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::EndGraphicRecording(PerWindowData* const p)
{
	if(!p->Recording)
		return;

	if(p->AntiAliasing == RendererAPI::AntiAliasing::MSAA) //Inject MSAA resolve pass
		MSAAResolvePass(p);

#ifndef TRAP_HEADLESS_MODE
	//Transition RenderTarget to Present
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({}, nullptr, nullptr, nullptr, nullptr,
																std::numeric_limits<uint32_t>::max(),
																std::numeric_limits<uint32_t>::max());
	TRAP::Ref<RenderTarget> presentRenderTarget = p->SwapChain->GetRenderTargets()[p->CurrentSwapChainImageIndex];
	RenderTargetBarrier barrier{presentRenderTarget, ResourceState::RenderTarget, ResourceState::Present};
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#endif

	//End Recording
	p->GraphicCommandBuffers[p->ImageIndex]->End();

	QueueSubmitDesc submitDesc{};
	submitDesc.Cmds = { p->GraphicCommandBuffers[p->ImageIndex] };
#ifndef TRAP_HEADLESS_MODE
	submitDesc.SignalSemaphores = { p->RenderCompleteSemaphores[p->ImageIndex] };
	submitDesc.WaitSemaphores = { p->ImageAcquiredSemaphore, p->ComputeCompleteSemaphores[p->ImageIndex] };
#else
	submitDesc.WaitSemaphores = { p->ComputeCompleteSemaphores[p->ImageIndex] };
#endif
	submitDesc.SignalFence = p->RenderCompleteFences[p->ImageIndex];
	s_graphicQueue->Submit(submitDesc);

#ifndef TRAP_HEADLESS_MODE
	QueuePresentDesc presentDesc{};
	presentDesc.Index = static_cast<uint8_t>(p->CurrentSwapChainImageIndex);
	presentDesc.WaitSemaphores = { p->RenderCompleteSemaphores[p->ImageIndex] };
	presentDesc.SwapChain = p->SwapChain;
	const PresentStatus presentStatus = s_graphicQueue->Present(presentDesc);
#endif

	p->ImageIndex = (p->ImageIndex + 1) % RendererAPI::ImageCount;

#ifndef TRAP_HEADLESS_MODE
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
		swapChainDesc.ColorFormat = SwapChain::GetRecommendedSwapchainFormat(true, false);
		swapChainDesc.ClearColor = p->ClearColor;
		swapChainDesc.ClearDepth = p->ClearDepth;
		swapChainDesc.ClearStencil = p->ClearStencil;
		swapChainDesc.EnableVSync = p->CurrentVSync;
		if(p->AntiAliasing == AntiAliasing::Off || p->AntiAliasing == AntiAliasing::MSAA)
			swapChainDesc.SampleCount = p->SampleCount;
		else
			swapChainDesc.SampleCount = SampleCount::One;
		p->SwapChain = SwapChain::Create(swapChainDesc);

		p->CurrentSwapChainImageIndex = 0;
		p->ImageIndex = 0;

		if (!p->SwapChain)
		{
			TRAP::Utils::Dialogs::ShowMsgBox("Swapchain creation failed", "Vulkan: Unable to create swapchain!\n"
											 "Error code: 0x0010",
			                                 TRAP::Utils::Dialogs::Style::Error,
											 TRAP::Utils::Dialogs::Buttons::Quit);
			TRAP::Application::Shutdown();
		}
	}
#else /*TRAP_HEADLESS_MODE*/
	if(p->Resize)
	{
		p->Resize = false;

		if(p->NewWidth > 0 && p->NewHeight > 0)
		{
			RendererAPI::RenderTargetDesc rTDesc{};
			rTDesc.Width = p->NewWidth;
			rTDesc.Height = p->NewHeight;
			rTDesc.Depth = 1;
			rTDesc.ArraySize = 1;
			rTDesc.MipLevels = 1;
			rTDesc.Format = SwapChain::GetRecommendedSwapchainFormat(true, false);
			rTDesc.StartState = RendererAPI::ResourceState::RenderTarget;
			rTDesc.SampleCount = SampleCount::One;
			for(uint32_t i = 0; i < RendererAPI::ImageCount; ++i)
			{
				p->RenderTargetsMSAA[i].reset();
				p->RenderTargets[i].reset();
				p->RenderTargets[i] = RenderTarget::Create(rTDesc);
			}
			if(p->AntiAliasing == AntiAliasing::MSAA)
			{
				rTDesc.SampleCount = p->SampleCount;
				for(uint32_t i = 0; i < RendererAPI::ImageCount; ++i)
					p->RenderTargetsMSAA[i] = RenderTarget::Create(rTDesc);
			}

			p->CurrentSwapChainImageIndex = 0;
			p->ImageIndex = 0;
		}
	}
#endif

	p->Recording = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::StartComputeRecording(PerWindowData* const p)
{
	TRAP_ASSERT(p);

	if(p->RecordingCompute)
		return;

	//Start Recording
	TRAP::Ref<Fence> computeCompleteFence = p->ComputeCompleteFences[p->ImageIndex];

	//Stall if CPU is running "Swap Chain Buffer Count" frames ahead of GPU
	if (computeCompleteFence->GetStatus() == FenceStatus::Incomplete)
		computeCompleteFence->Wait();

	//Reset cmd pool for this frame
	p->ComputeCommandPools[p->ImageIndex]->Reset();

	p->ComputeCommandBuffers[p->ImageIndex]->Begin();

	if(p->CurrentComputePipeline)
		p->ComputeCommandBuffers[p->ImageIndex]->BindPipeline(p->CurrentComputePipeline);

	p->RecordingCompute = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::EndComputeRecording(PerWindowData* const p)
{
	if(!p->RecordingCompute)
		return;

	//End Recording
	p->ComputeCommandBuffers[p->ImageIndex]->End();

	QueueSubmitDesc submitDesc{};
	submitDesc.Cmds = { p->ComputeCommandBuffers[p->ImageIndex] };
	submitDesc.WaitSemaphores = { p->GraphicsCompleteSemaphores[p->ImageIndex] };
	submitDesc.SignalSemaphores = { p->ComputeCompleteSemaphores[p->ImageIndex] };
	submitDesc.SignalFence = {p->ComputeCompleteFences[p->ImageIndex]};
	s_computeQueue->Submit(submitDesc);

	p->RecordingCompute = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::MSAAResolvePass(PerWindowData* const p)
{
#ifndef TRAP_HEADLESS_MODE
	TRAP::Ref<Graphics::RenderTarget> presentRT = p->SwapChain->GetRenderTargets()[p->CurrentSwapChainImageIndex];
	TRAP::Ref<Graphics::RenderTarget> MSAAResolveRT = p->SwapChain->GetRenderTargetsMSAA()[p->CurrentSwapChainImageIndex];
#else
	TRAP::Ref<Graphics::RenderTarget> presentRT = p->RenderTargets[p->CurrentSwapChainImageIndex];
	TRAP::Ref<Graphics::RenderTarget> MSAAResolveRT = p->RenderTargetsMSAA[p->CurrentSwapChainImageIndex];
#endif

	VulkanTexture* presentTex = dynamic_cast<VulkanTexture*>(presentRT->GetTexture());
	VulkanTexture* MSAAResolveTex = dynamic_cast<VulkanTexture*>(MSAAResolveRT->GetTexture());

	//Stop running render pass
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({}, nullptr, nullptr, nullptr, nullptr,
															   std::numeric_limits<uint32_t>::max(),
															   std::numeric_limits<uint32_t>::max());

	//Transition MSAAResolveRT from RenderTarget to CopySource
	RenderTargetBarrier barrier = {MSAAResolveRT, ResourceState::RenderTarget, ResourceState::CopySource};
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#ifndef TRAP_HEADLESS_MODE
	//Transition presentRT from Present to CopyDestination
	barrier = {presentRT, ResourceState::Present, ResourceState::CopyDestination};
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#else
	//Transition presentRT from RenderTarget to CopyDestination
	barrier = {presentRT, ResourceState::RenderTarget, ResourceState::CopyDestination};
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#endif

	VulkanCommandBuffer* vkCmdBuf = dynamic_cast<VulkanCommandBuffer*>(p->GraphicCommandBuffers[p->ImageIndex]);
	vkCmdBuf->ResolveImage(MSAAResolveTex, ResourceState::CopySource, presentTex, ResourceState::CopyDestination);

	//Transition presentRT from CopyDestination to RenderTarget
	barrier = {presentRT, ResourceState::CopyDestination, ResourceState::RenderTarget};
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#ifndef TRAP_HEADLESS_MODE
	//Transition MSAAResolveRT from CopySource to Present
	barrier = {MSAAResolveRT, ResourceState::CopySource, ResourceState::Present};
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#else
	//Transition MSAAResolveRT from CopySource to RenderTarget
	barrier = {MSAAResolveRT, ResourceState::CopySource, ResourceState::RenderTarget};
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#endif
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

	m_device = TRAP::MakeRef<VulkanDevice>(std::move(physicalDevice), SetupDeviceExtensions(physicalDevice.get()));

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
	PerWindowData* const p = s_perWindowDataMap[window].get();

	EndComputeRecording(p);
	EndGraphicRecording(p);
#ifndef TRAP_HEADLESS_MODE
	if (p->CurrentVSync != p->NewVSync) //Change V-Sync state only between frames!
	{
		if(p->SwapChain)
			p->SwapChain->ToggleVSync();
		p->CurrentVSync = p->NewVSync;
	}
	bool updateImGui = false;
#endif
	if (p->NewAntiAliasing != p->AntiAliasing || p->NewSampleCount != p->SampleCount) //Change sample count only between frames!
	{
		p->AntiAliasing = p->NewAntiAliasing;
		p->SampleCount = p->NewSampleCount;

		std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).SampleCount = p->SampleCount;
#ifndef TRAP_HEADLESS_MODE
		updateImGui = true;

		if(p->SwapChain)
		{
			if(p->AntiAliasing == RendererAPI::AntiAliasing::MSAA || p->AntiAliasing == RendererAPI::AntiAliasing::Off)
				p->SwapChain->SetSampleCount(p->SampleCount);
			else //Every other anti aliasing doesn't set the swapchains sample count
				p->SwapChain->SetSampleCount(RendererAPI::SampleCount::One);
		}
#else
		RendererAPI::RenderTargetDesc rTDesc{};
		rTDesc.Width = p->NewWidth;
		rTDesc.Height = p->NewHeight;
		rTDesc.Depth = 1;
		rTDesc.ArraySize = 1;
		rTDesc.MipLevels = 1;
		rTDesc.Format = SwapChain::GetRecommendedSwapchainFormat(true, false);
		rTDesc.StartState = RendererAPI::ResourceState::RenderTarget;
		rTDesc.SampleCount = SampleCount::One;
		for(uint32_t i = 0; i < RendererAPI::ImageCount; ++i)
		{
			p->RenderTargetsMSAA[i].reset();
			p->RenderTargets[i].reset();
			p->RenderTargets[i] = RenderTarget::Create(rTDesc);
		}
		if(p->AntiAliasing == AntiAliasing::MSAA)
		{
			rTDesc.SampleCount = p->SampleCount;
			for(uint32_t i = 0; i < RendererAPI::ImageCount; ++i)
				p->RenderTargetsMSAA[i] = RenderTarget::Create(rTDesc);
		}
#endif
	}

	StartComputeRecording(p);
	StartGraphicRecording(p);

#ifndef TRAP_HEADLESS_MODE
	if(updateImGui) //To set MSAA sample count for ImGui we need a running render pass.
		TRAP::Application::GetImGuiLayer().SetMSAASamples(static_cast<uint32_t>(p->SampleCount));
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Dispatch(std::array<uint32_t, 3> workGroupElements, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	if(!p->RecordingCompute)
		return;

	//Check if we have an actual shader bound
	if(!p->CurrentComputePipeline)
	{
		//Bind fallback shader
		this->BindShader(TRAP::Graphics::ShaderManager::Get("FallbackCompute"), p->Window);
	}

	//Calculate used work group sizes
	for(uint32_t i = 0; i < workGroupElements.size(); ++i)
		workGroupElements[i] = static_cast<uint32_t>(TRAP::Math::Round(workGroupElements[i] / p->CurrentComputeWorkGroupSize[i]));

	p->ComputeCommandBuffers[p->ImageIndex]->Dispatch(workGroupElements[0], workGroupElements[1], workGroupElements[2]);
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetVSync(const bool vsync, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	s_perWindowDataMap[window]->NewVSync = vsync;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetClearColor(const Math::Vec4& color, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	s_perWindowDataMap[window]->ClearColor = color;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetClearDepth(const float depth, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	s_perWindowDataMap[window]->ClearDepth = depth;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetClearStencil(const uint32_t stencil, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	s_perWindowDataMap[window]->ClearStencil = stencil;
}

//------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetResolution(const uint32_t width, const uint32_t height,
														Window* window)
{
	TRAP_ASSERT(width > 0 && height > 0, "Invalid render target resolution!");

	if(!window)
		window = TRAP::Application::GetWindow();

	auto* p = s_perWindowDataMap[window].get();
	p->Resize = true;
	p->NewWidth = width;
	p->NewHeight = height;
}
#endif

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthTesting(const bool enabled, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthTest = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthWriting(const bool enabled, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthWrite = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthFunction(const CompareMode function, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthFunc = function;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthFail(const StencilOp front, const StencilOp back, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

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
		window = TRAP::Application::GetWindow();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).RasterizerState->DepthBias = depthBias;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthBiasSlopeFactor(const float factor, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	std::get<GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).RasterizerState->SlopeScaledDepthBias = factor;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilTesting(const bool enabled, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

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
		window = TRAP::Application::GetWindow();

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
		window = TRAP::Application::GetWindow();

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
		window = TRAP::Application::GetWindow();

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
		window = TRAP::Application::GetWindow();

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
		window = TRAP::Application::GetWindow();

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
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RasterizerState->FillMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetPrimitiveTopology(const PrimitiveTopology topology, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).PrimitiveTopology = topology;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetFrontFace(const FrontFace face, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RasterizerState->FrontFace = face;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha,
                                                       Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);

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
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);

	gpd.BlendState->SrcFactors = {sourceRGB};
	gpd.BlendState->DstFactors = {destinationRGB};
	gpd.BlendState->SrcAlphaFactors = {sourceAlpha};
	gpd.BlendState->DstAlphaFactors = {destinationAlpha};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetShadingRate(const ShadingRate shadingRate,
						            					 TRAP::Graphics::Texture* texture,
		                            					 const ShadingRateCombiner postRasterizerRate,
							        					 const ShadingRateCombiner finalRate, Window* window)
{
	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	p->GraphicCommandBuffers[p->ImageIndex]->SetShadingRate(shadingRate, texture, postRasterizerRate, finalRate);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetAntiAliasing(const AntiAliasing antiAliasing, SampleCount sampleCount,
                                                          Window* window)
{
	TRAP_ASSERT(GPUSettings.MaxMSAASampleCount >= sampleCount, "Sample count is higher than max supported by GPU");

	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	if(antiAliasing == AntiAliasing::MSAA && sampleCount > GPUSettings.MaxMSAASampleCount)
		sampleCount = GPUSettings.MaxMSAASampleCount;
	else if(antiAliasing == AntiAliasing::MSAA && sampleCount == SampleCount::One)
		sampleCount = SampleCount::Two;
	else if(antiAliasing == AntiAliasing::Off)
		sampleCount = SampleCount::One;

	p->NewAntiAliasing = antiAliasing;
	p->NewSampleCount = sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::GetAntiAliasing(AntiAliasing& outAntiAliasing,
                                                          SampleCount& outSampleCount, Window* window)
{
	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	outAntiAliasing = p->NewAntiAliasing;
	outSampleCount = p->NewSampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Clear(const ClearBufferType clearType, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const data = s_perWindowDataMap[window].get();

	TRAP::Ref<RenderTarget> renderTarget;
#ifndef TRAP_HEADLESS_MODE
	if(data->AntiAliasing == RendererAPI::AntiAliasing::MSAA)
		renderTarget = data->SwapChain->GetRenderTargetsMSAA()[data->ImageIndex];
	else
		renderTarget = data->SwapChain->GetRenderTargets()[data->ImageIndex];
#else
	if(data->AntiAliasing == RendererAPI::AntiAliasing::MSAA)
		renderTarget = data->RenderTargetsMSAA[data->ImageIndex];
	else
		renderTarget = data->RenderTargets[data->ImageIndex];
#endif

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
		window = TRAP::Application::GetWindow();

	PerWindowData* const data = s_perWindowDataMap[window].get();

	data->GraphicCommandBuffers[data->ImageIndex]->SetViewport(static_cast<float>(x), static_cast<float>(y),
	                                                           static_cast<float>(width), static_cast<float>(height),
	                                                           minDepth, maxDepth);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width,
                                                     const uint32_t height, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const data = s_perWindowDataMap[window].get();

	data->GraphicCommandBuffers[data->ImageIndex]->SetScissor(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Draw(const uint32_t vertexCount, const uint32_t firstVertex,
                                               Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const data = s_perWindowDataMap[window].get();

	data->GraphicCommandBuffers[data->ImageIndex]->Draw(vertexCount, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::DrawIndexed(const uint32_t indexCount, const uint32_t firstIndex,
                                                      const uint32_t firstVertex, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const data = s_perWindowDataMap[window].get();

	data->GraphicCommandBuffers[data->ImageIndex]->DrawIndexed(indexCount, firstIndex, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::DrawInstanced(const uint32_t vertexCount, const uint32_t instanceCount,
                                                        const uint32_t firstVertex, const uint32_t firstInstance,
                                                        Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const data = s_perWindowDataMap[window].get();

	data->GraphicCommandBuffers[data->ImageIndex]->DrawInstanced(vertexCount, firstVertex, instanceCount,
	                                                             firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::DrawIndexedInstanced(const uint32_t indexCount, const uint32_t instanceCount,
                                                               const uint32_t firstIndex, const uint32_t firstInstance,
						                                       const uint32_t firstVertex, Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const data = s_perWindowDataMap[window].get();

	data->GraphicCommandBuffers[data->ImageIndex]->DrawIndexedInstanced(indexCount, firstIndex, instanceCount,
	                                                                    firstInstance, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindShader(Shader* shader, Window* window) const
{
	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const data = s_perWindowDataMap[window].get();

	const ShaderStage stages = shader->GetShaderStages();

	if (stages == ShaderStage::RayTracing)
	{
		//TODO RayTracingPipelineDesc.Pipeline.ShaderProgram = shader;
		//Bind pipeline
	}
	else if (stages == ShaderStage::Compute)
	{
		ComputePipelineDesc& cpd = std::get<ComputePipelineDesc>(data->ComputePipelineDesc.Pipeline);

		if(!shader->IsShaderValid())
		{
			//Overwrite invalid shader with fallback
			shader = TRAP::Graphics::ShaderManager::Get("FallbackCompute");

			cpd.ShaderProgram = shader;
			cpd.RootSignature = shader->GetRootSignature();
		}
		else if(shader->IsShaderValid() && cpd.ShaderProgram != shader)
		{
			cpd.ShaderProgram = shader;
			cpd.RootSignature = shader->GetRootSignature();
		}

		data->CurrentComputeWorkGroupSize.x = static_cast<float>(shader->GetNumThreadsPerGroup()[0]);
		data->CurrentComputeWorkGroupSize.y = static_cast<float>(shader->GetNumThreadsPerGroup()[1]);
		data->CurrentComputeWorkGroupSize.z = static_cast<float>(shader->GetNumThreadsPerGroup()[2]);

		data->CurrentComputePipeline = GetPipeline(data->ComputePipelineDesc);
		data->ComputeCommandBuffers[data->ImageIndex]->BindPipeline(data->CurrentComputePipeline);

		//Bind Descriptors
		for(uint32_t i = 0; i < RendererAPI::MaxDescriptorSets; ++i)
		{
			if(shader->GetDescriptorSets()[i])
			{
				data->ComputeCommandBuffers[data->ImageIndex]->BindDescriptorSet(i == 0 ? 0 : data->ImageIndex,
																			 	 *(shader->GetDescriptorSets()[i]));
			}
		}
	}
	else if (stages != ShaderStage::None && stages != ShaderStage::SHADER_STAGE_COUNT)
	{
		GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(data->GraphicsPipelineDesc.Pipeline);

		if(!shader->IsShaderValid())
		{
			//Overwrite invalid shader with fallback
			shader = TRAP::Graphics::ShaderManager::Get("FallbackGraphics");

			gpd.ShaderProgram = shader;
			gpd.RootSignature = shader->GetRootSignature();
		}
		else if(shader->IsShaderValid() && gpd.ShaderProgram != shader)
		{
			gpd.ShaderProgram = shader;
			gpd.RootSignature = shader->GetRootSignature();
		}

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
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

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
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	p->GraphicCommandBuffers[p->ImageIndex]->BindIndexBuffer(iBuffer, indexType, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindDescriptorSet(DescriptorSet& dSet, const uint32_t index,
															const QueueType queueType, Window* window)
{
	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "Invalid QueueType provided!");

	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	if(queueType == QueueType::Graphics)
		p->GraphicCommandBuffers[p->ImageIndex]->BindDescriptorSet(index, dSet);
	else if(queueType == QueueType::Compute)
		p->ComputeCommandBuffers[p->ImageIndex]->BindDescriptorSet(index, dSet);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindPushConstants(const char* name, const void* constantsData,
															const QueueType queueType, Window* window)
{
	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "Invalid QueueType provided!");

	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	if(queueType == QueueType::Graphics)
	{
		p->GraphicCommandBuffers[p->ImageIndex]->BindPushConstants
		(
			std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RootSignature,
			name, constantsData
		);
	}
	else if(queueType == QueueType::Compute)
	{
		p->ComputeCommandBuffers[p->ImageIndex]->BindPushConstants
		(
			std::get<ComputePipelineDesc>(p->ComputePipelineDesc.Pipeline).RootSignature,
			name, constantsData
		);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindPushConstantsByIndex(const uint32_t paramIndex,
                                                                   const void* constantsData,
																   const QueueType queueType,
																   Window* window)
{
	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "Invalid QueueType provided!");

	if (!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	if(queueType == QueueType::Graphics)
	{
		p->GraphicCommandBuffers[p->ImageIndex]->BindPushConstantsByIndex
		(
			std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RootSignature,
			paramIndex, constantsData
		);
	}
	else if(queueType == QueueType::Compute)
	{
		p->ComputeCommandBuffers[p->ImageIndex]->BindPushConstantsByIndex
		(
			std::get<ComputePipelineDesc>(p->ComputePipelineDesc.Pipeline).RootSignature,
			paramIndex, constantsData
		);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                                                   const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
							                               const RendererAPI::LoadActionsDesc* loadActions,
							                               std::vector<uint32_t>* colorArraySlices,
							                               std::vector<uint32_t>* colorMipSlices,
							                               const uint32_t depthArraySlice,
														   const uint32_t depthMipSlice, Window* window)
{
	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	std::vector<TRAP::Ref<Graphics::RenderTarget>> targets;
	if(colorTarget)
		targets.push_back(colorTarget);
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets
	(
		targets, depthStencil, loadActions, colorArraySlices, colorMipSlices, depthArraySlice, depthMipSlice
	);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                                                    const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
							                                const RendererAPI::LoadActionsDesc* loadActions,
							                                std::vector<uint32_t>* colorArraySlices,
							                                std::vector<uint32_t>* colorMipSlices,
							                                const uint32_t depthArraySlice,
														    const uint32_t depthMipSlice, Window* window)
{
	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets
	(
		colorTargets, depthStencil, loadActions, colorArraySlices, colorMipSlices, depthArraySlice, depthMipSlice
	);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
															    const QueueType queueType, Window* window)
{
	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "Invalid QueueType provided!");

	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	if(queueType == QueueType::Graphics)
		p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(&bufferBarrier, nullptr, nullptr);
	else if(queueType == QueueType::Compute)
		p->ComputeCommandBuffers[p->ImageIndex]->ResourceBarrier(&bufferBarrier, nullptr, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
																 const QueueType queueType, Window* window)
{
	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "Invalid QueueType provided!");

	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	if(queueType == QueueType::Graphics)
		p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(&bufferBarriers, nullptr, nullptr);
	else if(queueType == QueueType::Compute)
		p->ComputeCommandBuffers[p->ImageIndex]->ResourceBarrier(&bufferBarriers, nullptr, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
																 const QueueType queueType, Window* window)
{
	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "Invalid QueueType provided!");

	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	if(queueType == QueueType::Graphics)
		p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, &textureBarrier, nullptr);
	else if(queueType == QueueType::Compute)
		p->ComputeCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, &textureBarrier, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
																  const QueueType queueType, Window* window)
{
	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "Invalid QueueType provided!");

	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	if(queueType == QueueType::Graphics)
		p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, &textureBarriers, nullptr);
	else if(queueType == QueueType::Compute)
		p->ComputeCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, &textureBarriers, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
									                                  Window* window)
{
	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &renderTargetBarrier);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
									                                   Window* window)
{
	if(!window)
		window = TRAP::Application::GetWindow();

	PerWindowData* const p = s_perWindowDataMap[window].get();

	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &renderTargetBarriers);
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
		window = TRAP::Application::GetWindow();

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

//Helper function to generate screenshot data. See CaptureScreenshot.
void TRAP::Graphics::API::VulkanRenderer::MapRenderTarget(const TRAP::Ref<RenderTarget>& renderTarget,
														  ResourceState currResState, void* outPixelData)
{
	CommandPoolDesc cmdPoolDesc{};
	cmdPoolDesc.Queue = s_graphicQueue;
	cmdPoolDesc.Transient = true;
	TRAP::Ref<VulkanCommandPool> cmdPool = TRAP::MakeRef<VulkanCommandPool>(cmdPoolDesc);

	CommandBuffer* cmd = cmdPool->AllocateCommandBuffer(false);

	//Add a staging buffer
	uint16_t formatByteWidth = static_cast<uint16_t>(ImageFormatBitSizeOfBlock(renderTarget->GetImageFormat()) / 8u);
	BufferDesc bufferDesc{};
	bufferDesc.Descriptors = DescriptorType::RWBuffer;
	bufferDesc.MemoryUsage = ResourceMemoryUsage::GPUToCPU;
	bufferDesc.Size = static_cast<uint64_t>(renderTarget->GetWidth()) * renderTarget->GetHeight() * formatByteWidth;
	bufferDesc.Flags = BufferCreationFlags::PersistentMap | BufferCreationFlags::NoDescriptorViewCreation;
	bufferDesc.StartState = ResourceState::CopyDestination;
	TRAP::Ref<Buffer> buffer = Buffer::Create(bufferDesc);

	//Start recording
	cmd->Begin();

	//Transition the render target to the correct state
	RenderTargetBarrier srcBarrier = {renderTarget, currResState, ResourceState::CopySource};
	cmd->ResourceBarrier(nullptr, nullptr, &srcBarrier);

	uint32_t rowPitch = renderTarget->GetWidth() * formatByteWidth;
	const uint32_t width = renderTarget->GetTexture()->GetWidth();
	const uint32_t height = renderTarget->GetTexture()->GetHeight();
	const uint32_t depth = Math::Max(1u, renderTarget->GetTexture()->GetDepth());
	const ImageFormat fmt = renderTarget->GetTexture()->GetImageFormat();
	const uint32_t numBlocksWide = rowPitch / (ImageFormatBitSizeOfBlock(fmt) >> 3);

	//Copy the render target to the staging buffer
	uint32_t bufferRowLength = numBlocksWide * ImageFormatWidthOfBlock(fmt);
	VkImageSubresourceLayers layers{};
	layers.aspectMask = static_cast<VkImageAspectFlags>(renderTarget->GetTexture()->GetAspectMask());
	layers.mipLevel = 0;
	layers.baseArrayLayer = 0;
	layers.layerCount = 1;
	VkBufferImageCopy copy = API::VulkanInits::ImageCopy(bufferRowLength, width, height, depth, layers);

	VulkanCommandBuffer* vkCmd = dynamic_cast<VulkanCommandBuffer*>(cmd);
	VulkanTexture* vkTex = dynamic_cast<VulkanTexture*>(renderTarget->GetTexture());
	VulkanBuffer* vkBuf = dynamic_cast<VulkanBuffer*>(buffer.get());

	vkCmdCopyImageToBuffer(vkCmd->GetVkCommandBuffer(), vkTex->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vkBuf->GetVkBuffer(), 1, &copy);

	//Transition the render target back to the previous state
	srcBarrier = {renderTarget, ResourceState::CopySource, currResState};
	cmd->ResourceBarrier(nullptr, nullptr, &srcBarrier);

	//End recording
	cmd->End();

	//Submit the command buffer
	QueueSubmitDesc submitDesc{};
	submitDesc.Cmds = {cmd};

	s_graphicQueue->Submit(submitDesc);

	//Wait for work to finish on the GPU
	s_graphicQueue->WaitQueueIdle();

	//Copy to CPU memory.
	std::copy_n(static_cast<uint8_t*>(buffer->GetCPUMappedAddress()),
	            static_cast<std::size_t>(renderTarget->GetWidth()) * renderTarget->GetHeight() * formatByteWidth,
				static_cast<uint8_t*>(outPixelData));

	//Cleanup
	cmdPool->FreeCommandBuffer(cmd);
	cmdPool.reset();
	buffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Graphics::API::VulkanRenderer::CaptureScreenshot(Window* window)
{
	if(!window)
		window = TRAP::Application::GetWindow();

	auto* winData = s_perWindowDataMap[window].get();
	uint32_t lastFrame = (winData->ImageIndex - 1) % RendererAPI::ImageCount;

	//Wait for queues to finish
	s_computeQueue->WaitQueueIdle();
	s_graphicQueue->WaitQueueIdle();

#ifdef TRAP_HEADLESS_MODE
	TRAP::Ref<RenderTarget> rT = winData->RenderTargets[lastFrame];
#else
	TRAP::Ref<RenderTarget> rT = winData->SwapChain->GetRenderTargets()[lastFrame];
#endif

	const uint8_t channelCount = static_cast<uint8_t>(ImageFormatChannelCount(rT->GetImageFormat()));
	const bool hdr = ImageFormatIsFloat(rT->GetImageFormat());
	const bool u16 = ImageFormatIsU16(rT->GetImageFormat());
	const bool flipRedBlue = rT->GetImageFormat() != ImageFormat::R8G8B8A8_UNORM;

#ifdef TRAP_HEADLESS_MODE
	constexpr ResourceState resState = ResourceState::RenderTarget;
#else
	constexpr ResourceState resState = ResourceState::Present;
#endif

	std::vector<uint8_t> pixelDatau8{};
	std::vector<float> pixelDataf32{};
	std::vector<uint16_t> pixelDatau16{};
	if(!hdr && u16)
	{
		pixelDatau16.resize(static_cast<std::size_t>(rT->GetWidth()) *
						    static_cast<std::size_t>(rT->GetHeight()) *
						    static_cast<std::size_t>(channelCount));

		//Generate image data buffer
		MapRenderTarget(rT, resState, pixelDatau16.data());
	}
	else if(!hdr)
	{
		pixelDatau8.resize(static_cast<std::size_t>(rT->GetWidth()) *
						   static_cast<std::size_t>(rT->GetHeight()) *
						   static_cast<std::size_t>(channelCount));

		//Generate image data buffer
		MapRenderTarget(rT, resState, pixelDatau8.data());
	}
	else
	{
		pixelDataf32.resize(static_cast<std::size_t>(rT->GetWidth()) *
							static_cast<std::size_t>(rT->GetHeight()) *
							static_cast<std::size_t>(channelCount));

		//Generate image data buffer
		MapRenderTarget(rT, resState, pixelDataf32.data());
	}

	//Flip the BGRA to RGBA
	if(flipRedBlue)
	{
		for(uint32_t y = 0; y < rT->GetHeight(); ++y)
		{
			for(uint32_t x = 0; x < rT->GetWidth(); ++x)
			{
				uint32_t pixelIndex = (y * rT->GetWidth() + x) * channelCount;

				//Swap blue and red
				if(!hdr && u16)
				{
					uint16_t red = pixelDatau8[pixelIndex];
					pixelDatau16[pixelIndex] = pixelDatau8[pixelIndex + 2];
					pixelDatau16[pixelIndex + 2] = red;
				}
				else if(!hdr)
				{
					uint8_t red = pixelDatau8[pixelIndex];
					pixelDatau8[pixelIndex] = pixelDatau8[pixelIndex + 2];
					pixelDatau8[pixelIndex + 2] = red;
				}
				else
				{
					float red = pixelDataf32[pixelIndex];
					pixelDataf32[pixelIndex] = pixelDataf32[pixelIndex + 2];
					pixelDataf32[pixelIndex + 2] = red;
				}
			}
		}
	}

	if(pixelDatau8.empty() && pixelDataf32.empty()) //Error
		return TRAP::Image::LoadFromMemory(1, 1, TRAP::Image::ColorFormat::RGB, std::vector<uint8_t>{0, 0, 0});
	if(!pixelDataf32.empty()) //HDR
		return TRAP::Image::LoadFromMemory(rT->GetWidth(), rT->GetHeight(), static_cast<TRAP::Image::ColorFormat>(channelCount), pixelDataf32);
	if(!pixelDatau16.empty()) //U16
		return TRAP::Image::LoadFromMemory(rT->GetWidth(), rT->GetHeight(), static_cast<TRAP::Image::ColorFormat>(channelCount), pixelDatau16);

	//U8 Image
	return TRAP::Image::LoadFromMemory(rT->GetWidth(), rT->GetHeight(), static_cast<TRAP::Image::ColorFormat>(channelCount), pixelDatau8);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::InitPerWindowData(Window* window)
{
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
		p->GraphicsCompleteSemaphores[i] = Semaphore::Create();

		p->ComputeCommandPools[i] = CommandPool::Create({s_computeQueue, false});
		p->ComputeCommandBuffers[i] = p->ComputeCommandPools[i]->AllocateCommandBuffer(false);

		p->ComputeCompleteFences[i] = Fence::Create();
		p->ComputeCompleteSemaphores[i] = Semaphore::Create();
	}

	//Image Acquire Semaphore
	p->ImageAcquiredSemaphore = Semaphore::Create();

#ifndef TRAP_HEADLESS_MODE
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
		swapChainDesc.ColorFormat = SwapChain::GetRecommendedSwapchainFormat(true, false);
		swapChainDesc.EnableVSync = p->CurrentVSync;
		swapChainDesc.ClearColor = p->ClearColor;
		swapChainDesc.ClearDepth = p->ClearDepth;
		swapChainDesc.ClearStencil = p->ClearStencil;
		if(p->AntiAliasing == AntiAliasing::Off || p->AntiAliasing == AntiAliasing::MSAA)
			swapChainDesc.SampleCount = p->SampleCount;
		else
			swapChainDesc.SampleCount = SampleCount::One;
		p->SwapChain = SwapChain::Create(swapChainDesc);

		if (!p->SwapChain)
			TRAP::Application::Shutdown();

		StartComputeRecording(p.get());
		StartGraphicRecording(p.get());
	}
#else
	RendererAPI::RenderTargetDesc rTDesc{};
	rTDesc.Width = p->NewWidth;
	rTDesc.Height = p->NewHeight;
	rTDesc.Depth = 1;
	rTDesc.ArraySize = 1;
	rTDesc.MipLevels = 1;
	rTDesc.Format = SwapChain::GetRecommendedSwapchainFormat(true, false);
	rTDesc.StartState = RendererAPI::ResourceState::RenderTarget;
	rTDesc.SampleCount = SampleCount::One;
	for(uint32_t i = 0; i < RendererAPI::ImageCount; ++i)
		p->RenderTargets[i] = RenderTarget::Create(rTDesc);
	if(p->AntiAliasing == AntiAliasing::MSAA)
	{
		rTDesc.SampleCount = p->SampleCount;
		for(uint32_t i = 0; i < RendererAPI::ImageCount; ++i)
			p->RenderTargetsMSAA[i] = RenderTarget::Create(rTDesc);
	}

	StartComputeRecording(p.get());
	StartGraphicRecording(p.get());
#endif

	//Graphics Pipeline
#ifndef TRAP_HEADLESS_MODE
	const std::vector<TRAP::Ref<RenderTarget>>& rT = p->AntiAliasing == RendererAPI::AntiAliasing::MSAA ?
	                                                 p->SwapChain->GetRenderTargetsMSAA() :
													 p->SwapChain->GetRenderTargets();
#else
	const auto& rT = p->AntiAliasing == RendererAPI::AntiAliasing::MSAA ? p->RenderTargetsMSAA : p->RenderTargets;
#endif
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

	//Compute Pipeline
	p->ComputePipelineDesc = {};
	p->ComputePipelineDesc.Type = PipelineType::Compute;
	p->ComputePipelineDesc.Pipeline = ComputePipelineDesc();
	p->CurrentComputeWorkGroupSize = {1, 1, 1};

	s_perWindowDataMap[window] = std::move(p);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::RemovePerWindowData(Window* window)
{
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
	//Turn on all validations
	if (VulkanInstance::IsLayerSupported("VK_LAYER_KHRONOS_validation"))
		layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif

#ifdef USE_RENDER_DOC
	//Turn on render doc layer for gpu capture
	if(VulkanInstance::IsLayerSupported("VK_LAYER_RENDERDOC_Capture"))
		layers.emplace_back("VK_LAYER_RENDERDOC_Capture");
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
#ifndef TRAP_HEADLESS_MODE
		Utils::Dialogs::ShowMsgBox("Vulkan API error", "Mandatory Vulkan surface extensions are unsupported!\n"
								   "Error code: 0x0003", Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererVulkanPrefix, "Mandatory Vulkan surface extensions are unsupported!");
		TRAP::Application::Shutdown();
		exit(-1);
#endif
	}
	else
	{
		extensions.push_back(reqExt[0]);
		extensions.push_back(reqExt[1]);
	}

	//Vulkan 1.1 core
	//VK_KHR_get_physical_device_properties2
	//VK_KHR_external_memory_capabilities
	//VK_KHR_external_semaphore_capabilities
	//VK_KHR_external_fence_capabilities

#ifdef ENABLE_GRAPHICS_DEBUG
#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	if(VulkanInstance::IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		s_debugUtilsExtension = true;
	}
#else
	if(VulkanInstance::IsExtensionSupported(VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		s_debugUtilsExtension = true;
	}
#endif

#ifdef ENABLE_GPU_BASED_VALIDATION
	if(VulkanInstance::IsExtensionSupported(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
		s_validationFeaturesExtension = true;
	}
#endif
#endif

	///HDR support (requires surface extension)
	if (VulkanInstance::IsExtensionSupported(VK_KHR_SURFACE_EXTENSION_NAME) &&
	    VulkanInstance::IsExtensionSupported(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
		s_swapchainColorSpaceExtension = true;
	}

	return extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Graphics::API::VulkanRenderer::SetupDeviceExtensions(VulkanPhysicalDevice* const physicalDevice)
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

	//Vulkan 1.1 core
	//VK_KHR_maintenance1
	//VK_KHR_maintenance2
	//VK_KHR_maintenance3
	//VK_KHR_external_memory
	//VK_KHR_external_semaphore
	//VK_KHR_external_fence
	//VK_KHR_bind_memory2
	//VK_KHR_get_memory_requirements2
	//VK_KHR_sampler_ycbcr_conversion (needs check of capability flag)
	//VK_KHR_dedicated_allocation
	//VK_KHR_shader_draw_parameters (needs check of capability flag)
	//VK_KHR_multiview

	//Debug marker extension in case debug utils is not supported
#ifndef ENABLE_DEBUG_UTILS_EXTENSION
	if (physicalDevice->IsExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		s_debugMarkerSupport = true;
	}
#endif

	if (physicalDevice->IsExtensionSupported(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME);
		s_drawIndirectCountExtension = true;
	}

	if (physicalDevice->IsExtensionSupported(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
		s_descriptorIndexingExtension = true;
	}

	if (physicalDevice->IsExtensionSupported(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME);
		s_fragmentShaderInterlockExtension = true;
	}

	if (physicalDevice->IsExtensionSupported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
		s_bufferDeviceAddressExtension = true;
	}

	if (physicalDevice->IsExtensionSupported(VK_KHR_MAINTENANCE_4_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_MAINTENANCE_4_EXTENSION_NAME);
		s_maintenance4Extension = true;
	}

	if (physicalDevice->IsExtensionSupported(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
		s_memoryBudgetExtension = true;
	}

	if (physicalDevice->IsExtensionSupported(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME) &&
	    physicalDevice->IsExtensionSupported(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME);
		s_shadingRate = true;
	}

#ifdef TRAP_PLATFORM_WINDOWS
	if (physicalDevice->IsExtensionSupported(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
		s_externalMemoryWin32Extension = true;
	}
	if (physicalDevice->IsExtensionSupported(VK_KHR_EXTERNAL_FENCE_WIN32_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_EXTERNAL_FENCE_WIN32_EXTENSION_NAME);
		s_externalFenceWin32Extension = true;
	}
	if (physicalDevice->IsExtensionSupported(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME);
		s_externalSemaphoreWin32Extension = true;
	}
	s_externalMemory = s_externalMemoryWin32Extension && s_externalFenceWin32Extension &&
	                   s_externalSemaphoreWin32Extension;
#endif

	//RayTracing
	if(s_descriptorIndexingExtension &&
	   physicalDevice->IsExtensionSupported(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME) &&
	   s_bufferDeviceAddressExtension &&
	   physicalDevice->IsExtensionSupported(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME) &&
	   physicalDevice->IsExtensionSupported(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME) &&
	   physicalDevice->IsExtensionSupported(VK_KHR_SPIRV_1_4_EXTENSION_NAME) &&
	   physicalDevice->IsExtensionSupported(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) &&
	   physicalDevice->IsExtensionSupported(VK_KHR_RAY_QUERY_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
		s_rayTracingExtension = true;
	}

#ifdef ENABLE_NSIGHT_AFTERMATH
	if(physicalDevice->IsExtensionSupported(VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME);
		s_diagnosticsConfigSupport = true;
	}
	if(physicalDevice->IsExtensionSupported(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
		s_diagnosticCheckPointsSupport = true;
	}
#endif

	return extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::AddDefaultResources()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPrefix, "Creating DefaultResources");
#endif

	s_NullDescriptors = TRAP::MakeScope<NullDescriptors>();

	TRAP::Scope<TRAP::Graphics::API::VulkanTexture> vkTex = nullptr;

	//1D Texture
	TextureDesc textureDesc{};
	textureDesc.ArraySize = 1;
	textureDesc.Depth = 1;
	textureDesc.Format = ImageFormat::R8G8B8A8_UNORM;
	textureDesc.Height = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleCount = SampleCount::One;
	textureDesc.StartState = ResourceState::Common;
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.Width = 1;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1D)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1D)] = std::move(vkTex);

	//1D Texture Array
	textureDesc.ArraySize = 2;
	textureDesc.Descriptors = DescriptorType::Texture;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1DArray)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim1DArray)] = std::move(vkTex);

	//2D Texture
	textureDesc.Width = 2;
	textureDesc.Height = 2;
	textureDesc.ArraySize = 1;
	textureDesc.Descriptors = DescriptorType::Texture;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2D)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2D)] = std::move(vkTex);

	//2D MS Texture
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.SampleCount = SampleCount::Four;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DMS)] = std::move(vkTex);
	textureDesc.SampleCount = SampleCount::One;

	//2D Texture Array
	textureDesc.ArraySize = 2;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DArray)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DArray)] = std::move(vkTex);

	//2D MS Texture Array
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.SampleCount = SampleCount::Four;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim2DMSArray)] = std::move(vkTex);
	textureDesc.SampleCount = SampleCount::One;

	//3D Texture
	textureDesc.Depth = 2;
	textureDesc.ArraySize = 1;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim3D)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDim3D)] = std::move(vkTex);

	//Cube Texture
	textureDesc.Depth = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Descriptors = DescriptorType::TextureCube;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCube)] = std::move(vkTex);
	textureDesc.ArraySize = 6 * 2;
	vkTex = TRAP::MakeScope<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCubeArray)] = std::move(vkTex);

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

	TRAP::Ref<VulkanFence> fence = TRAP::MakeRef<VulkanFence>();

	s_NullDescriptors->InitialTransitionQueue = graphicsQueue;
	s_NullDescriptors->InitialTransitionCmdPool = cmdPool;
	s_NullDescriptors->InitialTransitionCmd = dynamic_cast<VulkanCommandBuffer*>(cmd);
	s_NullDescriptors->InitialTransitionFence = fence;

	//Transition resources
	for (uint32_t dim = 0; dim < static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount); ++dim)
	{
		if (s_NullDescriptors->DefaultTextureSRV[dim])
			UtilInitialTransition(s_NullDescriptors->DefaultTextureSRV[dim].get(), ResourceState::ShaderResource);

		if (s_NullDescriptors->DefaultTextureUAV[dim])
			UtilInitialTransition(s_NullDescriptors->DefaultTextureUAV[dim].get(), ResourceState::UnorderedAccess);
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

void TRAP::Graphics::API::VulkanRenderer::UtilInitialTransition(TRAP::Graphics::Texture* texture,
                                                                const RendererAPI::ResourceState startState)
{
	std::lock_guard<std::mutex> lock(s_NullDescriptors->InitialTransitionMutex);
	VulkanCommandBuffer* cmd = s_NullDescriptors->InitialTransitionCmd;
	s_NullDescriptors->InitialTransitionCmdPool->Reset();
	cmd->Begin();
	TextureBarrier barrier{texture, RendererAPI::ResourceState::Undefined, startState};
	cmd->ResourceBarrier(nullptr, &barrier, nullptr);
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

	const auto tempFolder = TRAP::FileSystem::GetGameTempFolderPath();
	if(tempFolder)
	{
		std::pair<std::unordered_map<uint64_t, TRAP::Ref<PipelineCache>>::iterator, bool> res;
		if (cacheIt == s_pipelineCaches.end())
		{
			PipelineCacheLoadDesc cacheDesc{};
			cacheDesc.Path = *tempFolder / (std::to_string(hash) + ".cache");
			res = s_pipelineCaches.try_emplace(hash, PipelineCache::Create(cacheDesc));
		}

		if(res.second) //Got inserted
			desc.Cache = res.first->second;
	}

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_TRACE(Log::RendererVulkanPipelinePrefix, "Recreating Graphics Pipeline...");
#endif
	TRAP::Ref<TRAP::Graphics::Pipeline> pipeline = Pipeline::Create(desc);
	const auto pipeRes = s_pipelines.try_emplace(hash, pipeline);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_TRACE(Log::RendererVulkanPipelinePrefix, "Cached Graphics Pipeline");
#endif
	return pipeRes.first->second;
}