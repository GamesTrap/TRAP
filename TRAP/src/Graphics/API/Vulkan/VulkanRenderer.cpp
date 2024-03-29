#include "TRAPPCH.h"
#include "VulkanRenderer.h"

#include "Application.h"
#include "Window/Window.h"
#include "Window/WindowingAPI.h"
#include "Utils/Utils.h"
#include "Utils/ErrorCodes/ErrorCodes.h"
#include "FileSystem/FileSystem.h"
#include "VulkanCommon.h"
#include "Layers/ImGui/ImGuiLayer.h"

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
#include "Objects/VulkanTexture.h"
#include "Objects/VulkanInits.h"
#include "Objects/VulkanShader.h"

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/PipelineDescHash.h"
#include "Graphics/API/Objects/Pipeline.h"
#include "Graphics/API/Objects/RenderTarget.h"
#include "Graphics/API/Objects/PipelineCache.h"
#include "Graphics/API/Objects/QueryPool.h"
#include "Graphics/API/Objects/SwapChain.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/Texture.h"

TRAP::Scope<TRAP::Graphics::API::VulkanRenderer::NullDescriptors> TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors = nullptr;

std::vector<VkPipelineColorBlendAttachmentState> TRAP::Graphics::API::VulkanRenderer::DefaultBlendAttachments(8);
VkPipelineRasterizationStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultRasterizerDesc = UtilToRasterizerDesc
(
	{CullMode::Back, {}, {}, {}, {}, {}}
);
VkPipelineDepthStencilStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultDepthDesc = UtilToDepthDesc
(
	{ false, false, CompareMode::GreaterOrEqual, {}, 0xFF, 0xFF, CompareMode::Always, {}, {}, {}, CompareMode::Always,
	  {}, {}, {} }
);
VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanRenderer::DefaultBlendDesc = UtilToBlendDesc
(
	{ {BlendConstant::One}, {BlendConstant::Zero}, {BlendConstant::One}, {BlendConstant::Zero}, {}, {},
	  {(BIT(0u) | BIT(1u) | BIT(2u) | BIT(3u)) }, BlendStateTargets::BlendStateTargetAll, false
	}, DefaultBlendAttachments
);

std::unordered_map<std::thread::id, TRAP::Graphics::API::VulkanRenderer::RenderPassMap> TRAP::Graphics::API::VulkanRenderer::s_renderPassMap{};
std::unordered_map<std::thread::id, TRAP::Graphics::API::VulkanRenderer::FrameBufferMap> TRAP::Graphics::API::VulkanRenderer::s_frameBufferMap{};

std::unordered_map<u64, TRAP::Ref<TRAP::Graphics::Pipeline>> TRAP::Graphics::API::VulkanRenderer::s_pipelines{};
std::unordered_map<u64,
                   TRAP::Ref<TRAP::Graphics::PipelineCache>> TRAP::Graphics::API::VulkanRenderer::s_pipelineCaches{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::~VulkanRenderer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TP_DEBUG(Log::RendererVulkanPrefix, "Destroying Renderer");

	s_descriptorPool.reset();

	s_ResourceLoader.reset();

	RemoveDefaultResources();

	std::lock_guard lock(s_renderPassMutex);
	LockMark(s_renderPassMutex);
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

			cache->Save(*tempFolder / fmt::format("{}.cache", hash));
		}
	}

	s_pipelineCaches.clear();

	//Free everything in order
	//Should happen automagically through Scope destructors
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::StartGraphicRecording(PerViewportData* const p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(p, "VulkanRenderer::StartGraphicRecording(): PerViewportData is nullptr!");

	if(p->Recording)
		return;

	//Start Recording
#ifndef TRAP_HEADLESS_MODE
	std::optional<u32> acquiredImage = p->SwapChain->AcquireNextImage(p->ImageAcquiredSemaphore, nullptr);

	if(!acquiredImage)
	{
		p->SwapChain->UpdateFramebufferSize();

		acquiredImage = p->SwapChain->AcquireNextImage(p->ImageAcquiredSemaphore, nullptr);
	}

	//Try again, if it also fails with the updated swapchain, quit engine
	if(!acquiredImage)
		Utils::DisplayError(Utils::ErrorCode::VulkanSwapchainImageAcquireFailed);

	p->CurrentSwapChainImageIndex = *acquiredImage;
#else
	p->CurrentSwapChainImageIndex = (p->CurrentSwapChainImageIndex + 1) % RendererAPI::ImageCount;
#endif

	const TRAP::Ref<Fence> renderCompleteFence = p->RenderCompleteFences[p->ImageIndex];

	//Stall if CPU is running "Swap Chain Buffer Count" frames ahead of GPU
	if (renderCompleteFence->GetStatus() == FenceStatus::Incomplete)
		renderCompleteFence->Wait();

	TRAP::Ref<RenderTarget> bindRenderTarget{};
	if(p->RenderScale != 1.0f || p->CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA)
		bindRenderTarget = p->InternalRenderTargets[p->CurrentSwapChainImageIndex];
	else
	{
#ifndef TRAP_HEADLESS_MODE
		bindRenderTarget = p->SwapChain->GetRenderTargets()[p->CurrentSwapChainImageIndex];
#else
		bindRenderTarget = p->RenderTargets[p->CurrentSwapChainImageIndex];
#endif
	}

	//Set Shading rate texture (check if texture is big enough, else it would create errors so just deactivate it)
#ifndef TRAP_HEADLESS_MODE
	const auto internalRes = GetInternalRenderResolution(p->Window);
#else
	const auto internalRes = GetInternalRenderResolution();
#endif /*TRAP_HEADLESS_MODE*/
	if(p->NewShadingRateTexture &&
	   p->NewShadingRateTexture->GetWidth() >= NumericCast<u32>(TRAP::Math::Ceil(NumericCast<f32>(internalRes.x()) / NumericCast<f32>(GPUSettings.ShadingRateTexelWidth))) &&
	   p->NewShadingRateTexture->GetHeight() >= NumericCast<u32>(TRAP::Math::Ceil(NumericCast<f32>(internalRes.y()) / NumericCast<f32>(GPUSettings.ShadingRateTexelHeight))))
	{
		p->CachedShadingRateTextures[p->ImageIndex] = p->NewShadingRateTexture;
		std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).ShadingRateTexture = p->NewShadingRateTexture;
	}
	else
		std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).ShadingRateTexture = nullptr;

	//Reset cmd pool for this frame
	p->GraphicCommandPools[p->ImageIndex]->Reset();

	p->GraphicCommandBuffers[p->ImageIndex]->Begin();

	BeginGPUFrameProfile(QueueType::Graphics, p);

#ifndef TRAP_HEADLESS_MODE
	RenderTargetBarrier barrier{p->SwapChain->GetRenderTargets()[p->CurrentSwapChainImageIndex], ResourceState::Present, ResourceState::RenderTarget};
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#endif /*TRAP_HEADLESS_MODE*/

	LoadActionsDesc loadActions{};
	std::get<0>(loadActions.LoadActionsColor) = LoadActionType::Clear;
	std::get<0>(loadActions.ClearColorValues) = p->ClearColor;
	loadActions.ClearDepthStencil = p->ClearDepthStencil;
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({ bindRenderTarget }, nullptr, &loadActions, nullptr, nullptr,
	                                                           std::numeric_limits<u32>::max(),
															   std::numeric_limits<u32>::max(), std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).ShadingRateTexture);

	//Set Default Dynamic Viewport & Scissor
	const u32 width = bindRenderTarget->GetWidth();
	const u32 height = bindRenderTarget->GetHeight();

	p->GraphicCommandBuffers[p->ImageIndex]->SetViewport(0.0f, 0.0f, NumericCast<f32>(width),
														 NumericCast<f32>(height), 0.0f, 1.0f);
	p->GraphicCommandBuffers[p->ImageIndex]->SetScissor(0, 0, width, height);
	if(p->CurrentGraphicsPipeline)
		p->GraphicCommandBuffers[p->ImageIndex]->BindPipeline(p->CurrentGraphicsPipeline);

	p->Recording = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::EndGraphicRecording(PerViewportData* const p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(!p->Recording)
		return;

	if(p->CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA && p->RenderScale == 1.0f) //Inject MSAA resolve pass
	{
#ifndef TRAP_HEADLESS_MODE
		const TRAP::Ref<RenderTarget> presentRenderTarget = p->SwapChain->GetRenderTargets()[p->CurrentSwapChainImageIndex];
#else
		const TRAP::Ref<RenderTarget> presentRenderTarget = p->RenderTargets[p->CurrentSwapChainImageIndex];
#endif
		const TRAP::Ref<RenderTarget> MSAARenderTarget = p->InternalRenderTargets[p->CurrentSwapChainImageIndex];

		Graphics::RendererAPI::GetRenderer()->MSAAResolvePass(MSAARenderTarget, presentRenderTarget, p->GraphicCommandBuffers[p->ImageIndex]);
	}

#ifndef TRAP_HEADLESS_MODE
	//Transition RenderTarget to Present
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({}, nullptr, nullptr, nullptr, nullptr,
																std::numeric_limits<u32>::max(),
																std::numeric_limits<u32>::max(), nullptr);
	const TRAP::Ref<RenderTarget> presentRenderTarget = p->SwapChain->GetRenderTargets()[p->CurrentSwapChainImageIndex];
	RenderTargetBarrier barrier{presentRenderTarget, ResourceState::RenderTarget, ResourceState::Present};
	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#else
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({}, nullptr, nullptr, nullptr, nullptr,
																std::numeric_limits<u32>::max(),
																std::numeric_limits<u32>::max(), nullptr);
#endif

	EndGPUFrameProfile(QueueType::Graphics, p);

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

	p->GraphicsFrameTime = ResolveGPUFrameProfile(QueueType::Graphics, p);

	p->Recording = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::StartComputeRecording(PerViewportData* const p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(p, "VulkanRenderer::StartComputeRecording(): PerViewportData is nullptr!");

	p->State = PerWindowState::PreUpdate;

	if(p->RecordingCompute)
		return;

	//Start Recording
	const TRAP::Ref<Fence> computeCompleteFence = p->ComputeCompleteFences[p->ImageIndex];

	//Stall if CPU is running "Swap Chain Buffer Count" frames ahead of GPU
	if (computeCompleteFence->GetStatus() == FenceStatus::Incomplete)
		computeCompleteFence->Wait();

	//Reset cmd pool for this frame
	p->ComputeCommandPools[p->ImageIndex]->Reset();

	p->ComputeCommandBuffers[p->ImageIndex]->Begin();

	BeginGPUFrameProfile(QueueType::Compute, p);

	if(p->CurrentComputePipeline)
		p->ComputeCommandBuffers[p->ImageIndex]->BindPipeline(p->CurrentComputePipeline);

	p->RecordingCompute = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::EndComputeRecording(PerViewportData* const p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(!p->RecordingCompute)
		return;

	EndGPUFrameProfile(QueueType::Compute, p);

	//End Recording
	p->ComputeCommandBuffers[p->ImageIndex]->End();

	QueueSubmitDesc submitDesc{};
	submitDesc.Cmds = { p->ComputeCommandBuffers[p->ImageIndex] };
	submitDesc.WaitSemaphores = { p->GraphicsCompleteSemaphores[p->ImageIndex] };
	submitDesc.SignalSemaphores = { p->ComputeCompleteSemaphores[p->ImageIndex] };
	submitDesc.SignalFence = {p->ComputeCompleteFences[p->ImageIndex]};
	s_computeQueue->Submit(submitDesc);

	p->ComputeFrameTime = ResolveGPUFrameProfile(QueueType::Compute, p);

	p->RecordingCompute = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Present(PerViewportData* const p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE

#ifdef NVIDIA_REFLEX_AVAILABLE
	GetRenderer()->ReflexMarker(Application::GetGlobalCounter(), PCLSTATS_PRESENT_START);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

	QueuePresentDesc presentDesc{};
	presentDesc.Index = p->CurrentSwapChainImageIndex;
	presentDesc.WaitSemaphores = { p->RenderCompleteSemaphores[p->ImageIndex] };
	presentDesc.SwapChain = p->SwapChain;
	const PresentStatus presentStatus = s_graphicQueue->Present(presentDesc);

#ifdef NVIDIA_REFLEX_AVAILABLE
	GetRenderer()->ReflexMarker(Application::GetGlobalCounter(), PCLSTATS_PRESENT_END);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

#endif /*TRAP_HEADLESS_MODE*/

	FrameMark;

	p->ImageIndex = (p->ImageIndex + 1) % RendererAPI::ImageCount;

#ifndef TRAP_HEADLESS_MODE
	if (presentStatus == PresentStatus::OutOfDate || p->ResizeSwapChain)
	{
		p->ResizeSwapChain = false;

		p->SwapChain->UpdateFramebufferSize();

		p->CurrentSwapChainImageIndex = 0;
		p->ImageIndex = 0;
	}
	else if (presentStatus == PresentStatus::DeviceReset || presentStatus == PresentStatus::Failed)
	{
		if (presentStatus == PresentStatus::DeviceReset)
			Utils::DisplayError(Utils::ErrorCode::VulkanDeviceReset);
		else if (presentStatus == PresentStatus::Failed)
			Utils::DisplayError(Utils::ErrorCode::VulkanPresentationFailed);
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
			for(u32 i = 0; i < RendererAPI::ImageCount; ++i)
				p->RenderTargets[i] = RenderTarget::Create(rTDesc);

			p->CurrentSwapChainImageIndex = 0;
			p->ImageIndex = 0;
		}
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::InitInternal(const std::string_view gameName)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	m_instance = TRAP::MakeRef<VulkanInstance>(gameName, SetupInstanceLayers(), SetupInstanceExtensions());
#ifdef ENABLE_GRAPHICS_DEBUG
	m_debug = TRAP::MakeScope<VulkanDebug>(m_instance);
#endif /*ENABLE_GRAPHICS_DEBUG*/

	const std::multimap<u32, TRAP::Utils::UUID> physicalDevices = VulkanPhysicalDevice::GetAllRatedPhysicalDevices(m_instance);
	TRAP::Scope<VulkanPhysicalDevice> physicalDevice;

	//Get Vulkan GPU UUID
	TRAP::Utils::UUID gpuUUID{};
	const std::optional<std::string> UUIDstr = TRAP::Application::GetConfig().Get<std::string>("VulkanGPU");
	if(UUIDstr)
		gpuUUID = TRAP::Utils::UUIDFromString(*UUIDstr);

	if(gpuUUID == TRAP::Utils::UUID{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0})
	{
		TP_ERROR(Log::RendererVulkanPrefix, "Invalid GPU UUID: \"", Utils::UUIDToString(gpuUUID), "\"!");
		TP_ERROR(Log::RendererVulkanPrefix, "Falling back to score based system");
		physicalDevice = TRAP::MakeScope<VulkanPhysicalDevice>(m_instance, (--physicalDevices.end())->second);
	}
	else
	{
		for (const auto& [score, devUUID] : physicalDevices)
		{
			if(gpuUUID != devUUID)
				continue;

			physicalDevice = TRAP::MakeScope<VulkanPhysicalDevice>(m_instance, devUUID);
			break;
		}

		if(!physicalDevice)
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Could not find a GPU with UUID: \"", Utils::UUIDToString(gpuUUID), "\"!");
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

	const VkPhysicalDeviceProperties& devProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties();
	const VkPhysicalDeviceDriverProperties& devDriverProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceDriverProperties();
	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");
	TP_INFO(Log::RendererVulkanPrefix, "Vulkan Instance Version: ",
		VK_VERSION_MAJOR(VulkanInstance::GetInstanceVersion().value_or(0)), '.',
		VK_VERSION_MINOR(VulkanInstance::GetInstanceVersion().value_or(0)), '.',
		VK_VERSION_PATCH(VulkanInstance::GetInstanceVersion().value_or(0)));
	TP_INFO(Log::RendererVulkanPrefix, "Vulkan Device Version: ", VK_VERSION_MAJOR(devProps.apiVersion), '.',
	        VK_VERSION_MINOR(devProps.apiVersion), '.', VK_VERSION_PATCH(devProps.apiVersion));
	TP_INFO(Log::RendererVulkanPrefix, "Driver Version: ", VK_VERSION_MAJOR(devProps.driverVersion), '.',
	        VK_VERSION_MINOR(devProps.driverVersion), '.', VK_VERSION_PATCH(devProps.driverVersion));
	TP_INFO(Log::RendererVulkanPrefix, "Driver: ", devDriverProps.driverName, " (", devDriverProps.driverInfo, ')');
	TP_INFO(Log::RendererVulkanPrefix, "Renderer: ", devProps.deviceName);
	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");

	m_rendererTitle = fmt::format("[Vulkan {}.{}.{}]",
	                              VK_VERSION_MAJOR(devProps.apiVersion),
	                              VK_VERSION_MINOR(devProps.apiVersion),
								  VK_VERSION_PATCH(devProps.apiVersion));
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::Flush(const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::Flush() const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::Flush(): Window is nullptr!");

	PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	ReflexMarker(Application::GetGlobalCounter(), PCLSTATS_RENDERSUBMIT_START);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

	EndComputeRecording(p);
	EndGraphicRecording(p);

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	ReflexMarker(Application::GetGlobalCounter(), PCLSTATS_RENDERSUBMIT_END);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

	Present(p);

	Sampler::UpdateSamplers();

#ifndef TRAP_HEADLESS_MODE
	if (p->CurrentVSync != p->NewVSync) //Change V-Sync state only between frames!
	{
		if(p->SwapChain)
			p->SwapChain->ToggleVSync();
		p->CurrentVSync = p->NewVSync;
	}
#endif /*TRAP_HEADLESS_MODE*/

	if(p->RenderScale != p->NewRenderScale)
		p->RenderScale = p->NewRenderScale;

	UpdateInternalRenderTargets(p);

	StartComputeRecording(p);
	StartGraphicRecording(p);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::Dispatch(std::array<u32, 3> workGroupElements, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::Dispatch(std::array<u32, 3> workGroupElements) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::Dispatch(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	if(!p->RecordingCompute)
		return;

	//Check if we have an actual shader bound
	if(!p->CurrentComputePipeline)
	{
		//Bind fallback shader
#ifndef TRAP_HEADLESS_MODE
		this->BindShader(TRAP::Graphics::ShaderManager::Get("FallbackCompute").get(), p->Window);
#else
		this->BindShader(TRAP::Graphics::ShaderManager::Get("FallbackCompute").get());
#endif /*TRAP_HEADLESS_MODE*/
	}

	//Calculate used work group sizes
	for(usize i = 0; i < workGroupElements.size(); ++i)
		workGroupElements[i] = NumericCast<u32>(TRAP::Math::Round(NumericCast<f32>(workGroupElements[i]) / NumericCast<f32>(p->CurrentComputeWorkGroupSize[i])));

	p->ComputeCommandBuffers[p->ImageIndex]->Dispatch(std::get<0>(workGroupElements), std::get<1>(workGroupElements), std::get<2>(workGroupElements));
}

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetVSync([[maybe_unused]] const bool vsync,
                                                   [[maybe_unused]] const Window* const window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(window, "VulkanRenderer::SetVSync(): Window is nullptr!");

	s_perViewportDataMap.at(window)->NewVSync = vsync;
}
#endif /*TRAP_HEADLESS_MODE*/

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetReflexFPSLimit([[maybe_unused]] const u32 limit)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef NVIDIA_REFLEX_AVAILABLE
	if(!GPUSettings.ReflexSupported)
		return;

	for(auto& [win, viewportData] : s_perViewportDataMap)
	{
		if(limit == 0)
			viewportData->SleepModeParams.minimumIntervalUs = 0;
		else
			viewportData->SleepModeParams.minimumIntervalUs = NumericCast<u32>((1000.0f / NumericCast<f32>(limit)) * 1000.0f);

		if(viewportData->SleepModeParams.bLowLatencyMode && viewportData->SleepModeParams.bLowLatencyBoost)
			SetLatencyMode(LatencyMode::EnabledBoost, win);
		else if(viewportData->SleepModeParams.bLowLatencyMode)
			SetLatencyMode(LatencyMode::Enabled, win);
		else
			SetLatencyMode(LatencyMode::Disabled, win);
	}
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}
#endif /*TRAP_HEADLESS_MODE*/

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetRenderScale(f32 scale, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetRenderScale(f32 scale) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::GetRenderScale(): Window is nullptr!");
#endif /*TRAP_HEADLESS_MODE*/
	TRAP_ASSERT(scale >= 0.5f && scale <= 2.0f, "VulkanRenderer::GetRenderScale(): Scale must be between 0.5f and 2.0f inclusive!");

	scale = TRAP::Math::Clamp(scale, 0.5f, 2.0f);

#ifndef TRAP_HEADLESS_MODE
	s_perViewportDataMap.at(window)->NewRenderScale = scale;
#else
	s_perViewportData->NewRenderScale = scale;
#endif /*TRAP_HEADLESS_MODE*/
}

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
f32 TRAP::Graphics::API::VulkanRenderer::GetRenderScale(const Window* const window) const
#else
f32 TRAP::Graphics::API::VulkanRenderer::GetRenderScale() const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::GetRenderScale(): Window is nullptr!");

	return s_perViewportDataMap.at(window)->RenderScale;
#else
	return s_perViewportData->RenderScale;
#endif /*TRAP_HEADLESS_MODE*/
}

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetClearColor(const RendererAPI::Color& color, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetClearColor(const RendererAPI::Color& color) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetClearColor(): Window is nullptr!");

	s_perViewportDataMap.at(window)->ClearColor = color;
#else
	s_perViewportData->ClearColor = color;
#endif /*TRAP_HEADLESS_MODE*/
}

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetClearDepth(const f32 depth, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetClearDepth(const f32 depth) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetClearDepth(): Window is nullptr!");

	s_perViewportDataMap.at(window)->ClearDepthStencil.Depth = depth;
#else
	s_perViewportData->ClearDepthStencil.Depth = depth;
#endif /*TRAP_HEADLESS_MODEs*/
}

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetClearStencil(const u32 stencil, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetClearStencil(const u32 stencil) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetClearStencil(): Window is nullptr!");

	s_perViewportDataMap.at(window)->ClearDepthStencil.Stencil = stencil;
#else
	s_perViewportData->ClearDepthStencil.Stencil = stencil;
#endif /*TRAP_HEADLESS_MODEs*/
}

//------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetResolution(const u32 width, const u32 height) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(width > 0 && height > 0, "VulkanRenderer::SetResolution(): Invalid render target resolution!");

	s_perViewportData->Resize = true;
	s_perViewportData->NewWidth = width;
	s_perViewportData->NewHeight = height;
}
#endif /*TRAP_HEADLESS_MODE*/

//------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::GetResolution(u32& width, u32& height) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	width = s_perViewportData->NewWidth;
	height = s_perViewportData->NewHeight;
}
#endif /*TRAP_HEADLESS_MODE*/

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthTesting(const bool enabled, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthTesting(const bool enabled) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetDepthTesting(): Window is nullptr!");

	std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthTest = enabled;
#else
	std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthTest = enabled;
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthWriting(const bool enabled, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthWriting(const bool enabled) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetDepthWriting(): Window is nullptr!");

	std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthWrite = enabled;
#else
	std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthWrite = enabled;
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthFunction(const CompareMode function, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthFunction(const CompareMode function) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetDepthFunction(): Window is nullptr!");

	std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthFunc = function;
#else
	std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	).DepthState->DepthFunc = function;
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthFail(const StencilOp front, const StencilOp back, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthFail(const StencilOp front, const StencilOp back) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetDepthFail(): Window is nullptr!");

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	);
#else
	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	);
#endif /*TRAP_HEADLESS_MODE*/

	gpd.DepthState->DepthFrontFail = front;
	gpd.DepthState->DepthBackFail = back;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthBias(const i32 depthBias, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthBias(const i32 depthBias) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetDepthBias(): Window is nullptr!");

	std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	).RasterizerState->DepthBias = depthBias;
#else
	std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	).RasterizerState->DepthBias = depthBias;
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthBiasSlopeFactor(const f32 factor, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthBiasSlopeFactor(const f32 factor) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetDepthBiasSlopeFactor(): Window is nullptr!");

	std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	).RasterizerState->SlopeScaledDepthBias = factor;
#else
	std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	).RasterizerState->SlopeScaledDepthBias = factor;
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetStencilTesting(const bool enabled, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetStencilTesting(const bool enabled) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetStencilTesting(): Window is nullptr!");

	std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	).DepthState->StencilTest = enabled;
#else
	std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	).DepthState->StencilTest = enabled;
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetStencilFail(const StencilOp front, const StencilOp back,
                                                         const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetStencilFail(const StencilOp front, const StencilOp back) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetStencilFail(): Window is nullptr!");

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	);
#else
	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	);
#endif /*TRAP_HEADLESS_MODE*/

	gpd.DepthState->StencilFrontFail = front;
	gpd.DepthState->StencilBackFail = back;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetStencilPass(const StencilOp front, const StencilOp back, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetStencilPass(const StencilOp front, const StencilOp back) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetStencilPass(): Window is nullptr!");

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	);
#else
	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	);
#endif /*TRAP_HEADLESS_MODE*/

	gpd.DepthState->StencilFrontPass = front;
	gpd.DepthState->StencilBackPass = back;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetStencilFunction(const CompareMode front, const CompareMode back,
                                                             const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetStencilFunction(const CompareMode front, const CompareMode back) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetStencilFunction(): Window is nullptr!");

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	);
#else
	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	);
#endif /*TRAP_HEADLESS_MODE*/

	gpd.DepthState->StencilFrontFunc = front;
	gpd.DepthState->StencilBackFunc = back;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetStencilMask(const u8 read, const u8 write, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetStencilMask(const u8 read, const u8 write) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetStencilMask(): Window is nullptr!");

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	);
#else
	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	);
#endif /*TRAP_HEADLESS_MODE*/

	gpd.DepthState->StencilReadMask = read;
	gpd.DepthState->StencilWriteMask = write;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetCullMode(const CullMode mode, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetCullMode(const CullMode mode) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetCullMode(): Window is nullptr!");

	std::get<GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	).RasterizerState->CullMode = mode;
#else
	std::get<GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	).RasterizerState->CullMode = mode;
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetFillMode(const FillMode mode, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetFillMode(const FillMode mode) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetFillMode(): Window is nullptr!");
#endif /*TRAP_HEADLESS_MODE*/

	if(mode != FillMode::Solid && !GPUSettings.FillModeNonSolid)
		return;

#ifndef TRAP_HEADLESS_MODE
	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RasterizerState->FillMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetPrimitiveTopology(const PrimitiveTopology topology, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetPrimitiveTopology(const PrimitiveTopology topology) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetPrimitiveTopology(): Window is nullptr!");

	PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).PrimitiveTopology = topology;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetFrontFace(const FrontFace face, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetFrontFace(const FrontFace face) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetFrontFace(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RasterizerState->FrontFace = face;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha,
                                                       const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetBlendMode(): Window is nullptr!");

	PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);

	gpd.BlendState->BlendModes = { modeRGB };
	gpd.BlendState->BlendAlphaModes = { modeAlpha };
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetBlendConstant(const BlendConstant sourceRGB,
														   const BlendConstant sourceAlpha,
                                                           const BlendConstant destinationRGB,
                                                           const BlendConstant destinationAlpha, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetBlendConstant(const BlendConstant sourceRGB,
														   const BlendConstant sourceAlpha,
                                                           const BlendConstant destinationRGB,
                                                           const BlendConstant destinationAlpha) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetBlendConstant(): Window is nullptr!");

	PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);

	gpd.BlendState->SrcFactors = {sourceRGB};
	gpd.BlendState->DstFactors = {destinationRGB};
	gpd.BlendState->SrcAlphaFactors = {sourceAlpha};
	gpd.BlendState->DstAlphaFactors = {destinationAlpha};
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetShadingRate(const ShadingRate shadingRate,
		                            					 ShadingRateCombiner postRasterizerRate,
							        					 ShadingRateCombiner finalRate, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetShadingRate(const ShadingRate shadingRate,
		                            					 ShadingRateCombiner postRasterizerRate,
							        					 ShadingRateCombiner finalRate) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(RendererAPI::GPUSettings.ShadingRateCaps != RendererAPI::ShadingRateCaps::NotSupported,
	            "VulkanRenderer::SetShadingRate(): Shading rate is not supported by this device!");

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetShadingRate(): Window is nullptr!");

	PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);

	if(gpd.DepthState->DepthWrite)
	{
		TRAP_ASSERT(false, "VulkanRenderer::SetShadingRate(): Depth write is enabled, shading rate cannot be set");
		TP_ERROR(Log::RendererVulkanPrefix, "VulkanRenderer::SetShadingRate(): Depth write is enabled, shading rate cannot be set");
		return;
	}

	if(RendererAPI::GPUSettings.ShadingRateCaps == RendererAPI::ShadingRateCaps::NotSupported) //VRS is not supported
		return;

	if(gpd.ShadingRateTexture)
		p->NewShadingRateTexture = nullptr;

	if((RendererAPI::GPUSettings.ShadingRateCaps & RendererAPI::ShadingRateCaps::PerDraw) != RendererAPI::ShadingRateCaps::NotSupported)
	{
		if(std::to_underlying(RendererAPI::GPUSettings.ShadingRateCombiner & postRasterizerRate) == 0u)
		{
			TP_ERROR(Log::RendererVulkanCommandBufferPrefix, "Shading rate combiner is not supported!");
			return;
		}
		if(std::to_underlying(RendererAPI::GPUSettings.ShadingRateCombiner & finalRate) == 0u)
		{
			TP_ERROR(Log::RendererVulkanCommandBufferPrefix, "Shading rate combiner is not supported!");
			return;
		}

		//VUID-vkCmdSetFragmentShadingRateKHR-primitiveFragmentShadingRate-04510
		if(std::to_underlying(RendererAPI::GPUSettings.ShadingRateCaps & RendererAPI::ShadingRateCaps::PerPrimitive) == 0u)
			postRasterizerRate = ShadingRateCombiner::Passthrough;
		//VUID-vkCmdSetFragmentShadingRateKHR-attachmentFragmentShadingRate-04511
		if(std::to_underlying(RendererAPI::GPUSettings.ShadingRateCaps & RendererAPI::ShadingRateCaps::PerTile) == 0u)
			finalRate = ShadingRateCombiner::Passthrough;

		gpd.ShadingRate = shadingRate;
		gpd.ShadingRateCombiners = {postRasterizerRate, finalRate};

		p->GraphicCommandBuffers[p->ImageIndex]->SetShadingRate(shadingRate, postRasterizerRate, finalRate);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetShadingRate(TRAP::Ref<TRAP::Graphics::RenderTarget> texture,
                                                         const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetShadingRate(TRAP::Ref<TRAP::Graphics::RenderTarget> texture) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(RendererAPI::GPUSettings.ShadingRateCaps != RendererAPI::ShadingRateCaps::NotSupported,
	            "VulkanRenderer::SetShadingRate(): Shading rate is not supported by this device!");

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetShadingRate(): Window is nullptr!");

	PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);

	if(gpd.DepthState->DepthWrite)
	{
		TRAP_ASSERT(false, "VulkanRenderer::SetShadingRate(): Depth write is enabled, shading rate cannot be set");
		TP_ERROR(Log::RendererVulkanPrefix, "VulkanRenderer::SetShadingRate(): Depth write is enabled, shading rate cannot be set");
		return;
	}

	if(RendererAPI::GPUSettings.ShadingRateCaps == RendererAPI::ShadingRateCaps::NotSupported) //VRS is not supported
		return;

	if((RendererAPI::GPUSettings.ShadingRateCaps & RendererAPI::ShadingRateCaps::PerTile) != RendererAPI::ShadingRateCaps::NotSupported)
	{
		gpd.ShadingRate = ShadingRate::Full;
		gpd.ShadingRateCombiners = {ShadingRateCombiner::Passthrough, ShadingRateCombiner::Override};

		p->GraphicCommandBuffers[p->ImageIndex]->SetShadingRate(gpd.ShadingRate,
																std::get<0>(gpd.ShadingRateCombiners),
																std::get<1>(gpd.ShadingRateCombiners));

		p->NewShadingRateTexture = texture;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::Clear(const ClearBufferType clearType, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::Clear(const ClearBufferType clearType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::Clear(): Window is nullptr!");

	const PerViewportData* const data = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const data = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	TRAP::Ref<RenderTarget> renderTarget;
	if(data->RenderScale != 1.0f || data->CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA)
		renderTarget = data->InternalRenderTargets[data->CurrentSwapChainImageIndex];
	else
	{
#ifndef TRAP_HEADLESS_MODE
		renderTarget = data->SwapChain->GetRenderTargets()[data->CurrentSwapChainImageIndex];
#else
		renderTarget = data->RenderTargets[data->CurrentSwapChainImageIndex];
#endif
	}

	if((clearType & ClearBufferType::Color) != ClearBufferType::NONE)
	{
		data->GraphicCommandBuffers[data->ImageIndex]->Clear(data->ClearColor, renderTarget->GetWidth(), renderTarget->GetHeight());
	}

	if((clearType & ClearBufferType::Depth_Stencil) != ClearBufferType::NONE &&
	   TRAP::Graphics::API::ImageFormatIsDepthAndStencil(renderTarget->GetImageFormat()))
	{
		data->GraphicCommandBuffers[data->ImageIndex]->Clear(data->ClearDepthStencil.Depth, data->ClearDepthStencil.Stencil,
															 renderTarget->GetWidth(), renderTarget->GetHeight());
	}
	else if((clearType & ClearBufferType::Depth) != ClearBufferType::NONE &&
	        (TRAP::Graphics::API::ImageFormatIsDepthAndStencil(renderTarget->GetImageFormat()) ||
			 TRAP::Graphics::API::ImageFormatIsDepthOnly(renderTarget->GetImageFormat())))
	{
		data->GraphicCommandBuffers[data->ImageIndex]->Clear(data->ClearDepthStencil.Depth, renderTarget->GetWidth(), renderTarget->GetHeight());
	}
	else if((clearType & ClearBufferType::Stencil) != ClearBufferType::NONE &&
	        TRAP::Graphics::API::ImageFormatHasStencil(renderTarget->GetImageFormat()))
	{
		data->GraphicCommandBuffers[data->ImageIndex]->Clear(data->ClearDepthStencil.Stencil, renderTarget->GetWidth(), renderTarget->GetHeight());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetViewport(const u32 x, const u32 y, const u32 width,
                                                      const u32 height, const f32 minDepth,
                                                      const f32 maxDepth, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetViewport(const u32 x, const u32 y, const u32 width,
                                                      const u32 height, const f32 minDepth,
                                                      const f32 maxDepth) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetViewport(): Window is nullptr!");
#endif /*TRAP_HEADLESS_MODE*/

	if (width == 0 || height == 0)
		return;

#ifndef TRAP_HEADLESS_MODE
	const PerViewportData* const data = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const data = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	data->GraphicCommandBuffers[data->ImageIndex]->SetViewport(NumericCast<f32>(x), NumericCast<f32>(y),
	                                                           NumericCast<f32>(width), NumericCast<f32>(height),
	                                                           minDepth, maxDepth);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetScissor(const u32 x, const u32 y, const u32 width,
                                                     const u32 height, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetScissor(const u32 x, const u32 y, const u32 width,
                                                     const u32 height) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::SetScissor(): Window is nullptr!");

	const PerViewportData* const data = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const data = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	data->GraphicCommandBuffers[data->ImageIndex]->SetScissor(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::Draw(const u32 vertexCount, const u32 firstVertex,
                                               const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::Draw(const u32 vertexCount, const u32 firstVertex) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::Draw(): Window is nullptr!");

	const PerViewportData* const data = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const data = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	data->GraphicCommandBuffers[data->ImageIndex]->Draw(vertexCount, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::DrawIndexed(const u32 indexCount, const u32 firstIndex,
                                                      const i32 firstVertex, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::DrawIndexed(const u32 indexCount, const u32 firstIndex,
                                                      const i32 firstVertex) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::DrawIndexed(): Window is nullptr!");

	const PerViewportData* const data = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const data = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	data->GraphicCommandBuffers[data->ImageIndex]->DrawIndexed(indexCount, firstIndex, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::DrawInstanced(const u32 vertexCount, const u32 instanceCount,
                                                        const u32 firstVertex, const u32 firstInstance,
                                                        const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::DrawInstanced(const u32 vertexCount, const u32 instanceCount,
                                                        const u32 firstVertex, const u32 firstInstance) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::DrawInstanced(): Window is nullptr!");

	const PerViewportData* const data = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const data = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	data->GraphicCommandBuffers[data->ImageIndex]->DrawInstanced(vertexCount, firstVertex, instanceCount,
	                                                             firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::DrawIndexedInstanced(const u32 indexCount, const u32 instanceCount,
                                                               const u32 firstIndex, const u32 firstInstance,
						                                       const i32 firstVertex, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::DrawIndexedInstanced(const u32 indexCount, const u32 instanceCount,
                                                               const u32 firstIndex, const u32 firstInstance,
						                                       const i32 firstVertex) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::DrawIndexedInstanced(): Window is nullptr!");

	const PerViewportData* const data = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const data = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	data->GraphicCommandBuffers[data->ImageIndex]->DrawIndexedInstanced(indexCount, firstIndex, instanceCount,
	                                                                    firstInstance, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindShader(Shader* shader, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindShader(Shader* shader) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::BindShader(): Window is nullptr!");

	PerViewportData* const data = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const data = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

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
			shader = TRAP::Graphics::ShaderManager::Get("FallbackCompute").get();

			cpd.ShaderProgram = shader;
			cpd.RootSignature = shader->GetRootSignature();
		}
		else if(shader->IsShaderValid() && cpd.ShaderProgram != shader)
		{
			cpd.ShaderProgram = shader;
			cpd.RootSignature = shader->GetRootSignature();
		}

		data->CurrentComputeWorkGroupSize.x() = std::get<0>(shader->GetNumThreadsPerGroup());
		data->CurrentComputeWorkGroupSize.y() = std::get<1>(shader->GetNumThreadsPerGroup());
		data->CurrentComputeWorkGroupSize.z() = std::get<2>(shader->GetNumThreadsPerGroup());

		data->CurrentComputePipeline = GetPipeline(data->ComputePipelineDesc);
		data->ComputeCommandBuffers[data->ImageIndex]->BindPipeline(data->CurrentComputePipeline);

		//Bind Descriptors
		for(u32 i = 0; i < RendererAPI::MaxDescriptorSets; ++i)
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
			shader = TRAP::Graphics::ShaderManager::Get("FallbackGraphics").get();

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
		for(u32 i = 0; i < RendererAPI::MaxDescriptorSets; ++i)
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

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer,
                                                           const VertexBufferLayout& layout, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer,
                                                           const VertexBufferLayout& layout) const
#endif /*TRAP_HEADLESS_MODE*/
{
	static constexpr auto ShaderDataTypeToImageFormat = [](const ShaderDataType s) -> ImageFormat
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

		case ShaderDataType::Int:
			return ImageFormat::R32_SINT;

		case ShaderDataType::Int2:
			return ImageFormat::R32G32_SINT;

		case ShaderDataType::Int3:
			return ImageFormat::R32G32B32_SINT;

		case ShaderDataType::Int4:
			return ImageFormat::R32G32B32A32_SINT;

		case ShaderDataType::UInt:
			return ImageFormat::R32_SINT;

		case ShaderDataType::UInt2:
			return ImageFormat::R32G32_SINT;

		case ShaderDataType::UInt3:
			return ImageFormat::R32G32B32_SINT;

		case ShaderDataType::UInt4:
			return ImageFormat::R32G32B32A32_SINT;

		case ShaderDataType::None:
			[[fallthrough]];
		default:
			return ImageFormat::Undefined;
		}
	};

	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::BindVertexBuffer(): Window is nullptr!");

	PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	p->GraphicCommandBuffers[p->ImageIndex]->BindVertexBuffer({ vBuffer }, { layout.GetStride() }, {});

	const TRAP::Ref<VertexLayout> lay = TRAP::MakeRef<VertexLayout>();
	const std::vector<VertexBufferElement>& elements = layout.GetElements();
	lay->AttributeCount = NumericCast<u32>(elements.size());
	for(usize i = 0; i < elements.size(); ++i)
	{
		lay->Attributes[i].Binding = 0;
		lay->Attributes[i].Location = NumericCast<u32>(i);
		lay->Attributes[i].Format = ShaderDataTypeToImageFormat(elements[i].Type);
		lay->Attributes[i].Rate = VertexAttributeRate::Vertex;
		lay->Attributes[i].Offset = elements[i].Offset;
	}
	std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).VertexLayout = lay;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer,
                                                          const IndexType indexType, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer,
                                                          const IndexType indexType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::BindIndexBuffer(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	p->GraphicCommandBuffers[p->ImageIndex]->BindIndexBuffer(iBuffer, indexType, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindDescriptorSet(DescriptorSet& dSet, const u32 index,
															const QueueType queueType, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindDescriptorSet(DescriptorSet& dSet, const u32 index,
															const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::BindDescriptorSet(): Invalid QueueType provided!");

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::BindDescriptorSet(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	if(queueType == QueueType::Graphics)
		p->GraphicCommandBuffers[p->ImageIndex]->BindDescriptorSet(index, dSet);
	else if(queueType == QueueType::Compute)
		p->ComputeCommandBuffers[p->ImageIndex]->BindDescriptorSet(index, dSet);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindPushConstants(const std::string_view name, const void* constantsData,
		                                                    const usize constantsLength,
															const QueueType queueType, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindPushConstants(const std::string_view name, const void* constantsData,
		                                                    const usize constantsLength,
															const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::BindPushConstants(): Invalid QueueType provided!");

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::BindPushConstants(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	if(queueType == QueueType::Graphics)
	{
		p->GraphicCommandBuffers[p->ImageIndex]->BindPushConstants
		(
			std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RootSignature,
			name, constantsData, constantsLength
		);
	}
	else if(queueType == QueueType::Compute)
	{
		p->ComputeCommandBuffers[p->ImageIndex]->BindPushConstants
		(
			std::get<ComputePipelineDesc>(p->ComputePipelineDesc.Pipeline).RootSignature,
			name, constantsData, constantsLength
		);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindPushConstantsByIndex(const u32 paramIndex,
                                                                   const void* constantsData,
		                                                           const usize constantsLength,
																   const QueueType queueType,
																   const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindPushConstantsByIndex(const u32 paramIndex,
                                                                   const void* constantsData,
		                                                           usize constantsLength,
																   const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::BindPushConstantsByIndex(): Invalid QueueType provided!");

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::BindPushConstantsByIndex(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	if(queueType == QueueType::Graphics)
	{
		p->GraphicCommandBuffers[p->ImageIndex]->BindPushConstantsByIndex
		(
			std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline).RootSignature,
			paramIndex, constantsData, constantsLength
		);
	}
	else if(queueType == QueueType::Compute)
	{
		p->ComputeCommandBuffers[p->ImageIndex]->BindPushConstantsByIndex
		(
			std::get<ComputePipelineDesc>(p->ComputePipelineDesc.Pipeline).RootSignature,
			paramIndex, constantsData, constantsLength
		);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                                                   const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
							                               const RendererAPI::LoadActionsDesc* const loadActions,
							                               std::vector<u32>* const colorArraySlices,
							                               std::vector<u32>* const colorMipSlices,
							                               const u32 depthArraySlice,
														   const u32 depthMipSlice, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                                                   const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
							                               const RendererAPI::LoadActionsDesc* const loadActions,
							                               std::vector<u32>* const colorArraySlices,
							                               std::vector<u32>* const colorMipSlices,
							                               const u32 depthArraySlice,
														   const u32 depthMipSlice) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::BindRenderTarget(): Window is nullptr!");

	PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	TRAP::Ref<TRAP::Graphics::RenderTarget> shadingRateTex = nullptr;

	//We may needs to change the graphics pipeline if RenderTargetCount or ColorFormats don't match
	if(colorTarget)
	{
		GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);
		shadingRateTex = gpd.ShadingRateTexture;

		gpd.RenderTargetCount = 1;
		gpd.ColorFormats = {colorTarget->GetImageFormat()};
#ifndef TRAP_HEADLESS_MODE
		BindShader(gpd.ShaderProgram, window);
#else
		BindShader(gpd.ShaderProgram);
#endif /*TRAP_HEADLESS_MODE*/
	}

	std::vector<TRAP::Ref<Graphics::RenderTarget>> targets;
	if(colorTarget)
		targets.push_back(colorTarget);
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets
	(
		targets, depthStencil, loadActions, colorArraySlices, colorMipSlices, depthArraySlice, depthMipSlice, shadingRateTex
	);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                                                    const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
							                                const RendererAPI::LoadActionsDesc* const loadActions,
							                                std::vector<u32>* const colorArraySlices,
							                                std::vector<u32>* const colorMipSlices,
							                                const u32 depthArraySlice,
														    const u32 depthMipSlice, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                                                    const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
							                                const RendererAPI::LoadActionsDesc* const loadActions,
							                                std::vector<u32>* const colorArraySlices,
							                                std::vector<u32>* const colorMipSlices,
							                                const u32 depthArraySlice,
														    const u32 depthMipSlice) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::BindRenderTargets(): Window is nullptr!");

	PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	TRAP::Ref<TRAP::Graphics::RenderTarget> shadingRateTex = nullptr;

	//We may needs to change the graphics pipeline if RenderTargetCount or ColorFormats don't match
	if(!colorTargets.empty())
	{
		GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);
		shadingRateTex = gpd.ShadingRateTexture;

		gpd.RenderTargetCount = NumericCast<u32>(colorTargets.size());
		gpd.ColorFormats.resize(colorTargets.size());
		for(usize i = 0; i < colorTargets.size(); ++i)
			gpd.ColorFormats[i] = colorTargets[i]->GetImageFormat();
#ifndef TRAP_HEADLESS_MODE
		BindShader(gpd.ShaderProgram, window);
#else
		BindShader(gpd.ShaderProgram);
#endif /*TRAP_HEADLESS_MODE*/
	}

	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets
	(
		colorTargets, depthStencil, loadActions, colorArraySlices, colorMipSlices, depthArraySlice, depthMipSlice, shadingRateTex
	);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
															    const QueueType queueType, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
															    const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::ResourceBufferBarrier(): Invalid QueueType provided!");

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::ResourceBufferBarrier(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	if(queueType == QueueType::Graphics)
		p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(&bufferBarrier, nullptr, nullptr);
	else if(queueType == QueueType::Compute)
		p->ComputeCommandBuffers[p->ImageIndex]->ResourceBarrier(&bufferBarrier, nullptr, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
																 const QueueType queueType, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
																 const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::ResourceBufferBarriers(): Invalid QueueType provided!");

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::ResourceBufferBarriers(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	if(queueType == QueueType::Graphics)
		p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(&bufferBarriers, nullptr, nullptr);
	else if(queueType == QueueType::Compute)
		p->ComputeCommandBuffers[p->ImageIndex]->ResourceBarrier(&bufferBarriers, nullptr, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
																 const QueueType queueType, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
																 const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::ResourceTextureBarrier(): Invalid QueueType provided!");

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::ResourceTextureBarrier(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	if(queueType == QueueType::Graphics)
		p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, &textureBarrier, nullptr);
	else if(queueType == QueueType::Compute)
		p->ComputeCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, &textureBarrier, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
																  const QueueType queueType, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
																  const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::ResourceTextureBarriers(): Invalid QueueType provided!");

	#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::ResourceTextureBarriers(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	if(queueType == QueueType::Graphics)
		p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, &textureBarriers, nullptr);
	else if(queueType == QueueType::Compute)
		p->ComputeCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, &textureBarriers, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
									                                  const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::ResourceRenderTargetBarrier(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &renderTargetBarrier);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
									                                   const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::ResourceRenderTargetBarriers(): Window is nullptr!");

	const PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	const PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	p->GraphicCommandBuffers[p->ImageIndex]->ResourceBarrier(nullptr, nullptr, &renderTargetBarriers);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ReflexSleep() const
{
#ifdef NVIDIA_REFLEX_AVAILABLE
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(!GPUSettings.ReflexSupported)
		return;

	u64 signalValue = 0;
	VkCall(vkGetSemaphoreCounterValueKHR(m_device->GetVkDevice(), m_device->GetReflexSemaphore(), &signalValue));
	++signalValue;

	VkSemaphoreWaitInfoKHR waitInfo = VulkanInits::SemaphoreWaitInfo(m_device->GetReflexSemaphore(), signalValue);
	VkReflexCall(NvLL_VK_Sleep(m_device->GetVkDevice(), signalValue));
	VkCall(vkWaitSemaphoresKHR(m_device->GetVkDevice(), &waitInfo, std::numeric_limits<u64>::max()));
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ReflexMarker([[maybe_unused]] const u32 frame,
                                                       [[maybe_unused]] const u32 marker) const
{
#ifdef NVIDIA_REFLEX_AVAILABLE
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	PCLSTATS_MARKER(marker, frame);

	if(!GPUSettings.ReflexSupported)
		return;

	if(marker == PCLSTATS_PC_LATENCY_PING)
		return;

	if(marker == PCLSTATS_TRIGGER_FLASH) //BUG This gives ERROR_DEVICE_LOST
		return;

	NVLL_VK_LATENCY_MARKER_PARAMS params{};
	params.frameID = frame;
	params.markerType = static_cast<NVLL_VK_LATENCY_MARKER_TYPE>(marker);
	VkReflexCall(NvLL_VK_SetLatencyMarker(m_device->GetVkDevice(), &params));
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
[[nodiscard]] NVLL_VK_LATENCY_RESULT_PARAMS TRAP::Graphics::API::VulkanRenderer::ReflexGetLatency() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	NVLL_VK_LATENCY_RESULT_PARAMS params{};

	VkReflexCall(NvLL_VK_GetLatency(m_device->GetVkDevice(), &params));

	return params;
}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] bool TRAP::Graphics::API::VulkanRenderer::GetVSync(const Window* const window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(window, "VulkanRenderer::GetVSync(): Window is nullptr!");

	return s_perViewportDataMap.at(window)->CurrentVSync;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::UUID TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUUUID() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_device->GetPhysicalDevice()->GetPhysicalDeviceUUID();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUName() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().deviceName;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::GPUVendor TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUVendor() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_device->GetPhysicalDevice()->GetVendor();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<std::pair<std::string, TRAP::Utils::UUID>> TRAP::Graphics::API::VulkanRenderer::GetAllGPUs() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
														  const ResourceState currResState, void* const outPixelData)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	CommandPoolDesc cmdPoolDesc{};
	cmdPoolDesc.Queue = s_graphicQueue;
	cmdPoolDesc.Transient = true;
	TRAP::Ref<VulkanCommandPool> cmdPool = TRAP::MakeRef<VulkanCommandPool>(cmdPoolDesc);

	CommandBuffer* const cmd = cmdPool->AllocateCommandBuffer(false);

	//Add a staging buffer
	const u32 formatByteWidth = ImageFormatBitSizeOfBlock(renderTarget->GetImageFormat()) / 8u;
	BufferDesc bufferDesc{};
	bufferDesc.Descriptors = DescriptorType::RWBuffer;
	bufferDesc.MemoryUsage = ResourceMemoryUsage::GPUToCPU;
	bufferDesc.Size = NumericCast<u64>(renderTarget->GetWidth()) * renderTarget->GetHeight() * formatByteWidth;
	bufferDesc.Flags = BufferCreationFlags::PersistentMap | BufferCreationFlags::NoDescriptorViewCreation;
	bufferDesc.StartState = ResourceState::CopyDestination;
	bufferDesc.QueueType = QueueType::Graphics;
	TRAP::Ref<Buffer> buffer = Buffer::Create(bufferDesc);

	//Start recording
	cmd->Begin();

	//Transition the render target to the correct state
	RenderTargetBarrier srcBarrier = {renderTarget, currResState, ResourceState::CopySource};
	cmd->ResourceBarrier(nullptr, nullptr, &srcBarrier);

	//TODO Replace with CommandBuffer::CopySubresource()
	const u32 rowPitch = renderTarget->GetWidth() * formatByteWidth;
	const u32 width = renderTarget->GetTexture()->GetWidth();
	const u32 height = renderTarget->GetTexture()->GetHeight();
	const u32 depth = Math::Max(1u, renderTarget->GetTexture()->GetDepth());
	const ImageFormat fmt = renderTarget->GetTexture()->GetImageFormat();
	const u32 numBlocksWide = rowPitch / (ImageFormatBitSizeOfBlock(fmt) >> 3u);

	//Copy the render target to the staging buffer
	const u32 bufferRowLength = numBlocksWide * ImageFormatWidthOfBlock(fmt);
	VkImageSubresourceLayers layers{};
	layers.aspectMask = static_cast<VkImageAspectFlags>(renderTarget->GetTexture()->GetAspectMask());
	layers.mipLevel = 0;
	layers.baseArrayLayer = 0;
	layers.layerCount = 1;
	const VkBufferImageCopy copy = API::VulkanInits::ImageCopy(bufferRowLength, width, height, depth, layers);

	const VulkanCommandBuffer* const vkCmd = dynamic_cast<VulkanCommandBuffer*>(cmd);
	const Ref<VulkanTexture> vkTex = std::dynamic_pointer_cast<VulkanTexture>(renderTarget->GetTexture());
	const Ref<VulkanBuffer> vkBuf = std::dynamic_pointer_cast<VulkanBuffer>(buffer);

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
	std::copy_n(static_cast<u8*>(buffer->GetCPUMappedAddress()),
	            NumericCast<usize>(renderTarget->GetWidth()) * renderTarget->GetHeight() * formatByteWidth,
				static_cast<u8*>(outPixelData));

	//Cleanup
	cmdPool->FreeCommandBuffer(cmd);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Graphics::API::VulkanRenderer::CaptureScreenshot(const Window* const window) const
#else
[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Graphics::API::VulkanRenderer::CaptureScreenshot() const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::CaptureScreenshot(): Window is nullptr!");

	const auto* const viewportData = s_perViewportDataMap.at(window).get();
#else
	const auto* const viewportData = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/
	const u32 lastFrame = (viewportData->ImageIndex - 1) % RendererAPI::ImageCount;

	//Wait for queues to finish
	s_computeQueue->WaitQueueIdle();
	s_graphicQueue->WaitQueueIdle();

#ifdef TRAP_HEADLESS_MODE
	const TRAP::Ref<RenderTarget> rT = viewportData->RenderTargets[lastFrame];
#else
	const TRAP::Ref<RenderTarget> rT = viewportData->SwapChain->GetRenderTargets()[lastFrame];
#endif

	const u32 channelCount = ImageFormatChannelCount(rT->GetImageFormat());
	const bool hdr = ImageFormatIsFloat(rT->GetImageFormat());
	const bool isUnsigned16 = ImageFormatIsU16(rT->GetImageFormat());
	const bool flipRedBlue = rT->GetImageFormat() != ImageFormat::R8G8B8A8_UNORM;

#ifdef TRAP_HEADLESS_MODE
	static constexpr ResourceState resState = ResourceState::RenderTarget;
#else
	static constexpr ResourceState resState = ResourceState::Present;
#endif

	std::vector<u8> pixelDatau8{};
	std::vector<f32> pixelDataf32{};
	std::vector<u16> pixelDatau16{};
	if(!hdr && isUnsigned16)
	{
		pixelDatau16.resize(NumericCast<usize>(rT->GetWidth()) *
						    NumericCast<usize>(rT->GetHeight()) *
						    NumericCast<usize>(channelCount));

		//Generate image data buffer
		MapRenderTarget(rT, resState, pixelDatau16.data());
	}
	else if(!hdr)
	{
		pixelDatau8.resize(NumericCast<usize>(rT->GetWidth()) *
						   NumericCast<usize>(rT->GetHeight()) *
						   NumericCast<usize>(channelCount));

		//Generate image data buffer
		MapRenderTarget(rT, resState, pixelDatau8.data());
	}
	else
	{
		pixelDataf32.resize(NumericCast<usize>(rT->GetWidth()) *
							NumericCast<usize>(rT->GetHeight()) *
							NumericCast<usize>(channelCount));

		//Generate image data buffer
		MapRenderTarget(rT, resState, pixelDataf32.data());
	}

	//Flip the BGRA to RGBA
	if(flipRedBlue)
	{
		for(u32 y = 0; y < rT->GetHeight(); ++y)
		{
			for(u32 x = 0; x < rT->GetWidth(); ++x)
			{
				const u32 pixelIndex = (y * rT->GetWidth() + x) * channelCount;

				//Swap blue and red
				if(!hdr && isUnsigned16)
				{
					const u16 red = pixelDatau8[pixelIndex];
					pixelDatau16[pixelIndex] = pixelDatau8[pixelIndex + 2];
					pixelDatau16[pixelIndex + 2] = red;
				}
				else if(!hdr)
				{
					const u8 red = pixelDatau8[pixelIndex];
					pixelDatau8[pixelIndex] = pixelDatau8[pixelIndex + 2];
					pixelDatau8[pixelIndex + 2] = red;
				}
				else
				{
					const f32 red = pixelDataf32[pixelIndex];
					pixelDataf32[pixelIndex] = pixelDataf32[pixelIndex + 2];
					pixelDataf32[pixelIndex + 2] = red;
				}
			}
		}
	}

	if(pixelDatau8.empty() && pixelDataf32.empty()) //Error
		return nullptr;

	if(!pixelDataf32.empty()) //HDR
		return TRAP::Image::LoadFromMemory(rT->GetWidth(), rT->GetHeight(), static_cast<TRAP::Image::ColorFormat>(channelCount), pixelDataf32);
	if(!pixelDatau16.empty()) //U16
		return TRAP::Image::LoadFromMemory(rT->GetWidth(), rT->GetHeight(), static_cast<TRAP::Image::ColorFormat>(channelCount), pixelDatau16);

	//U8 Image
	return TRAP::Image::LoadFromMemory(rT->GetWidth(), rT->GetHeight(), static_cast<TRAP::Image::ColorFormat>(channelCount), pixelDatau8);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::MSAAResolvePass(const TRAP::Ref<RenderTarget> source,
                                                          const TRAP::Ref<RenderTarget> destination,
														  CommandBuffer* const cmd) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(source, "VulkanRenderer::MSAAResolvePass(): source is nullptr!");
	TRAP_ASSERT(destination, "VulkanRenderer::MSAAResolvePass(): destination is nullptr!");
	TRAP_ASSERT(source->GetSampleCount() != SampleCount::One, "VulkanRenderer::MSAAResolvePass(): source does not use MSAA!");
	TRAP_ASSERT(destination->GetSampleCount() == SampleCount::One, "VulkanRenderer::MSAAResolvePass(): destination must not use MSAA!");
	TRAP_ASSERT(cmd, "VulkanRenderer::MSAAResolvePass(): cmd is nullptr!");

	Ref<VulkanTexture> dstTex = std::dynamic_pointer_cast<VulkanTexture>(destination->GetTexture());
	Ref<VulkanTexture> MSAATex = std::dynamic_pointer_cast<VulkanTexture>(source->GetTexture());

	//Stop running render pass
	cmd->BindRenderTargets({}, nullptr, nullptr, nullptr, nullptr,
						   std::numeric_limits<u32>::max(),
						   std::numeric_limits<u32>::max(), nullptr);

	//Transition source from RenderTarget to CopySource
	RenderTargetBarrier barrier = {source, ResourceState::RenderTarget, ResourceState::CopySource};
	cmd->ResourceBarrier(nullptr, nullptr, &barrier);
	//Transition destination from RenderTarget to CopyDestination
	barrier = {destination, ResourceState::RenderTarget, ResourceState::CopyDestination};
	cmd->ResourceBarrier(nullptr, nullptr, &barrier);

	const VulkanCommandBuffer* const vkCmdBuf = dynamic_cast<VulkanCommandBuffer*>(cmd);
	vkCmdBuf->ResolveImage(MSAATex, ResourceState::CopySource, dstTex, ResourceState::CopyDestination);

	//Transition destination from CopyDestination to RenderTarget
	barrier = {destination, ResourceState::CopyDestination, ResourceState::RenderTarget};
	cmd->ResourceBarrier(nullptr, nullptr, &barrier);
	//Transition source from CopySource to RenderTarget
	barrier = {source, ResourceState::CopySource, ResourceState::RenderTarget};
	cmd->ResourceBarrier(nullptr, nullptr, &barrier);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::UpdateInternalRenderTargets(PerViewportData* const viewportData) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(viewportData, "VulkanRenderer::UpdateInternalRenderTargets(): viewportData is nullptr!");

	bool rebuild = false;
#ifndef TRAP_HEADLESS_MODE
	const auto newInternalRes = GetInternalRenderResolution(viewportData->Window);
#else
	const auto newInternalRes = GetInternalRenderResolution();
#endif /*TRAP_HEADLESS_MODE*/
	if(viewportData->InternalRenderTargets.empty())
		rebuild = true;
	else if(viewportData->InternalRenderTargets[0] &&
	        (viewportData->InternalRenderTargets[0]->GetWidth() != newInternalRes.x() ||
	         viewportData->InternalRenderTargets[0]->GetHeight() != newInternalRes.y()))
	{
		rebuild = true;
	}

	if (viewportData->CurrentAntiAliasing != s_newAntiAliasing ||
	    viewportData->CurrentSampleCount != s_newSampleCount) //Change sample count only between frames!
	{
		viewportData->CurrentAntiAliasing = s_newAntiAliasing;
		viewportData->CurrentSampleCount = s_newSampleCount;

		if(viewportData->CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA)
			std::get<GraphicsPipelineDesc>(viewportData->GraphicsPipelineDesc.Pipeline).SampleCount = viewportData->CurrentSampleCount;
		else
			std::get<GraphicsPipelineDesc>(viewportData->GraphicsPipelineDesc.Pipeline).SampleCount = SampleCount::One;

		rebuild = true;
	}

	if(rebuild)
	{
		WaitIdle();

		viewportData->InternalRenderTargets = {};

		RendererAPI::RenderTargetDesc rTDesc{};
		rTDesc.Width = newInternalRes.x();
		rTDesc.Height = newInternalRes.y();
		rTDesc.Depth = 1;
		rTDesc.ArraySize = 1;
		rTDesc.MipLevels = 1;
		rTDesc.Format = SwapChain::GetRecommendedSwapchainFormat(true, false);
		rTDesc.StartState = RendererAPI::ResourceState::RenderTarget;
		rTDesc.SampleCount = (viewportData->CurrentAntiAliasing == AntiAliasing::MSAA) ? viewportData->CurrentSampleCount : SampleCount::One;

#ifndef TRAP_HEADLESS_MODE
		const u32 imageCount = NumericCast<u32>(viewportData->SwapChain->GetRenderTargets().size());
#else
		static constexpr u32 imageCount = RendererAPI::ImageCount;
#endif /*TRAP_HEADLESS_MODE*/
		viewportData->InternalRenderTargets.resize(imageCount);
		for(u32 i = 0; i < imageCount; ++i)
			viewportData->InternalRenderTargets[i] = RenderTarget::Create(rTDesc);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::RenderScalePass(TRAP::Ref<RenderTarget> source,
                                                          TRAP::Ref<RenderTarget> destination,
		                                                  const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::RenderScalePass(TRAP::Ref<RenderTarget> source,
                                                          TRAP::Ref<RenderTarget> destination) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanRenderer::RenderScalePass(): Window is nullptr!");

	PerViewportData* const p = s_perViewportDataMap.at(window).get();
#else
	PerViewportData* const p = s_perViewportData.get();
#endif /*TRAP_HEADLESS_MODE*/

	TRAP_ASSERT(p->RenderScale != 1.0f, "VulkanRenderer::RenderScalePass(): RenderScale must not be 1.0f!");

	VulkanCommandBuffer* const cmd = dynamic_cast<VulkanCommandBuffer*>(p->GraphicCommandBuffers[p->ImageIndex]);

	//Stop running render pass
	p->GraphicCommandBuffers[p->ImageIndex]->BindRenderTargets({}, nullptr, nullptr, nullptr, nullptr,
															   std::numeric_limits<u32>::max(),
															   std::numeric_limits<u32>::max());

	if(source->GetSampleCount() != SampleCount::One) //Extra work to resolve MSAA
	{
		TRAP::Ref<RenderTarget> tempTarget = p->TemporaryResolveRenderTargets[p->ImageIndex];

		bool updateTempTarget = !tempTarget;
		if(tempTarget &&
		   (tempTarget->GetWidth() != source->GetWidth() ||
	 	    tempTarget->GetHeight() != source->GetHeight()))
		{
			updateTempTarget = true;
		}

		if(updateTempTarget)
		{
			//Create temporary resolve render target
			RenderTargetDesc rTDesc{};
			rTDesc.Width = source->GetWidth();
			rTDesc.Height = source->GetHeight();
			rTDesc.Depth = 1;
			rTDesc.ArraySize = 1;
			rTDesc.MipLevels = 1;
			rTDesc.Format = SwapChain::GetRecommendedSwapchainFormat(true, false);
			rTDesc.StartState = RendererAPI::ResourceState::RenderTarget;
			rTDesc.SampleCount = SampleCount::One;
			tempTarget = RenderTarget::Create(rTDesc);

			p->TemporaryResolveRenderTargets[p->ImageIndex] = tempTarget;
		}

		//1. Do MSAAResolvePass() to resolve the multi sampled image into a single sampled image with same resolution
		MSAAResolvePass(source, tempTarget, cmd);

		source = tempTarget;
	}

	//Transition layout for copying
	TextureBarrier barrier{};
	barrier.Texture = source->GetTexture().get();
	barrier.CurrentState = ResourceState::RenderTarget;
	barrier.NewState = ResourceState::CopySource;
#ifndef TRAP_HEADLESS_MODE
	ResourceTextureBarrier(barrier, QueueType::Graphics, window);
#else
	ResourceTextureBarrier(barrier, QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
	barrier.Texture = destination->GetTexture().get();
	barrier.CurrentState = ResourceState::RenderTarget;
	barrier.NewState = ResourceState::CopyDestination;
#ifndef TRAP_HEADLESS_MODE
	ResourceTextureBarrier(barrier, QueueType::Graphics, window);
#else
	ResourceTextureBarrier(barrier, QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/

	//Do scaling
	TRAP::Ref<VulkanTexture> texInternal = std::dynamic_pointer_cast<VulkanTexture>(source->GetTexture());
	TRAP::Ref<VulkanTexture> texOutput = std::dynamic_pointer_cast<VulkanTexture>(destination->GetTexture());

	VkImage internal = texInternal->GetVkImage();
	VkImage output = texOutput->GetVkImage();

	VkImageBlit region{};
	region.srcOffsets[0] = {0, 0, 0};
	region.srcOffsets[1] = { NumericCast<i32>(texInternal->GetWidth()), NumericCast<i32>(texInternal->GetHeight()), 1};
	region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.srcSubresource.mipLevel = 0;
	region.srcSubresource.baseArrayLayer = 0;
	region.srcSubresource.layerCount = 1;
	region.dstOffsets[0] = {0, 0, 0};
	region.dstOffsets[1] = {NumericCast<i32>(texOutput->GetWidth()), NumericCast<i32>(texOutput->GetHeight()), 1};
	region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.dstSubresource.mipLevel = 0;
	region.dstSubresource.baseArrayLayer = 0;
	region.dstSubresource.layerCount = 1;

	vkCmdBlitImage(cmd->GetVkCommandBuffer(), internal, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, output,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VK_FILTER_LINEAR);

	//Transition layout back
	barrier.Texture = source->GetTexture().get();
	barrier.CurrentState = ResourceState::CopySource;
	barrier.NewState = ResourceState::RenderTarget;
#ifndef TRAP_HEADLESS_MODE
	ResourceTextureBarrier(barrier, QueueType::Graphics, window);
#else
	ResourceTextureBarrier(barrier, QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
	barrier.Texture = destination->GetTexture().get();
	barrier.CurrentState = ResourceState::CopyDestination;
	barrier.NewState = ResourceState::RenderTarget;
#ifndef TRAP_HEADLESS_MODE
	ResourceTextureBarrier(barrier, QueueType::Graphics, window);
#else
	ResourceTextureBarrier(barrier, QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/

	//Start render pass
	cmd->BindRenderTargets({destination}, nullptr, nullptr, nullptr, nullptr,
							std::numeric_limits<u32>::max(),
							std::numeric_limits<u32>::max());

	cmd->SetViewport(0.0f, 0.0f, NumericCast<f32>(destination->GetWidth()), NumericCast<f32>(destination->GetHeight()), 0.0f, 1.0f);
	cmd->SetScissor(0, 0, destination->GetWidth(), destination->GetHeight());
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetLatencyMode([[maybe_unused]] const LatencyMode mode,
                                                         [[maybe_unused]] const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(window, "VulkanRenderer::SetLatencyMode(): Window is nullptr!");

#ifdef NVIDIA_REFLEX_AVAILABLE
	if(!GPUSettings.ReflexSupported)
		return;

	[[maybe_unused]] PerViewportData* p = s_perViewportDataMap.at(window).get();

	p->SleepModeParams.bLowLatencyMode = mode != LatencyMode::Disabled;
	p->SleepModeParams.bLowLatencyBoost = mode == LatencyMode::EnabledBoost;

	VkReflexCall(NvLL_VK_SetSleepMode(m_device->GetVkDevice(), &p->SleepModeParams));
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Graphics::RendererAPI::LatencyMode TRAP::Graphics::API::VulkanRenderer::GetLatencyMode([[maybe_unused]] const Window* const window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(window, "VulkanRenderer::GetLatencyMode(): Window is nullptr!");

	if(!GPUSettings.ReflexSupported)
		return LatencyMode::Disabled;

#ifdef NVIDIA_REFLEX_AVAILABLE
	[[maybe_unused]] PerViewportData* p = s_perViewportDataMap.at(window).get();

	NVLL_VK_GET_SLEEP_STATUS_PARAMS params{};
	VkReflexCall(NvLL_VK_GetSleepStatus(m_device->GetVkDevice(), &params));

	if(!params.bLowLatencyMode)
	{
		p->SleepModeParams.bLowLatencyMode = false;
		p->SleepModeParams.bLowLatencyBoost = false;
	}

	if(p->SleepModeParams.bLowLatencyMode && p->SleepModeParams.bLowLatencyBoost)
		return LatencyMode::EnabledBoost;
	if(p->SleepModeParams.bLowLatencyMode)
		return LatencyMode::Enabled;
#endif /*NVIDIA_REFLEX_AVAILABLE*/

	return LatencyMode::Disabled;
}

#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::InitPerViewportData(Window* const window, const bool VSyncEnabled) const
#else
void TRAP::Graphics::API::VulkanRenderer::InitPerViewportData(const u32 width, const u32 height) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	if (s_perViewportDataMap.contains(window)) //Window is already in map
		return;
#endif /*TRAP_HEADLESS_MODE*/

	//Add new Window to map
	TRAP::Scope<PerViewportData> p = TRAP::MakeScope<PerViewportData>();

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	if(Application::GetFPSLimit() == 0)
		p->SleepModeParams.minimumIntervalUs = 0;
	else
		p->SleepModeParams.minimumIntervalUs = NumericCast<u32>(((1000.0f / NumericCast<f32>(Application::GetFPSLimit())) * 1000.0f));
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	p->Window = window;
#else
	p->NewWidth = width;
	p->NewHeight = height;
#endif /*TRAP_HEADLESS_MODE*/

	p->GraphicsFrameTime = 0.0f;
	p->ComputeFrameTime = 0.0f;

	const RendererAPI::QueryPoolDesc queryPoolDesc{QueryType::Timestamp, 1 * 2};
	RendererAPI::BufferDesc bufferDesc{};
	bufferDesc.MemoryUsage = RendererAPI::ResourceMemoryUsage::GPUToCPU;
	bufferDesc.Flags = RendererAPI::BufferCreationFlags::OwnMemory;
	bufferDesc.Size = sizeof(u64) * 1 * 2;
	bufferDesc.StartState = RendererAPI::ResourceState::CopyDestination;

	//For each buffered image
	for (u32 i = 0; i < RendererAPI::ImageCount; ++i)
	{
		//Graphics
		//Create Graphic Command Pool
		p->GraphicCommandPools[i] = CommandPool::Create({ s_graphicQueue, false });
		//Allocate Graphic Command Buffer
		p->GraphicCommandBuffers[i] = p->GraphicCommandPools[i]->AllocateCommandBuffer(false);

		//Create Render Fences/Semaphores
		p->RenderCompleteFences[i] = Fence::Create();
		p->RenderCompleteSemaphores[i] = Semaphore::Create();
		p->GraphicsCompleteSemaphores[i] = Semaphore::Create();

		p->GraphicsTimestampQueryPools[i] = QueryPool::Create(queryPoolDesc);
		bufferDesc.Name = "GPU FrameTime Graphics Query Readback Buffer";
		RendererAPI::BufferLoadDesc loadDesc{};
		loadDesc.Desc = bufferDesc;
		GetResourceLoader()->AddResource(loadDesc, nullptr);
		p->GraphicsTimestampReadbackBuffers[i] = loadDesc.Buffer;

		//Compute
		p->ComputeCommandPools[i] = CommandPool::Create({s_computeQueue, false});
		p->ComputeCommandBuffers[i] = p->ComputeCommandPools[i]->AllocateCommandBuffer(false);

		p->ComputeCompleteFences[i] = Fence::Create();
		p->ComputeCompleteSemaphores[i] = Semaphore::Create();

		p->ComputeTimestampQueryPools[i] = QueryPool::Create(queryPoolDesc);
		bufferDesc.Name = "GPU FrameTime Compute Query Readback Buffer";
		loadDesc = {};
		loadDesc.Desc = bufferDesc;
		GetResourceLoader()->AddResource(loadDesc, nullptr);
		p->ComputeTimestampReadbackBuffers[i] = loadDesc.Buffer;
	}

#ifndef TRAP_HEADLESS_MODE
	//Image Acquire Semaphore
	p->ImageAcquiredSemaphore = Semaphore::Create();

	//Create Swapchain
	p->CurrentVSync = p->NewVSync = VSyncEnabled;
	SwapChainDesc swapChainDesc{};
	swapChainDesc.Window = window;
	swapChainDesc.PresentQueues = { s_graphicQueue };
	swapChainDesc.Width = window->GetFrameBufferSize().x();
	swapChainDesc.Height = window->GetFrameBufferSize().y();
	swapChainDesc.ImageCount = RendererAPI::ImageCount;
	swapChainDesc.ColorFormat = SwapChain::GetRecommendedSwapchainFormat(true, false);
	swapChainDesc.EnableVSync = p->CurrentVSync;
	swapChainDesc.ClearValue = p->ClearColor;
	swapChainDesc.SampleCount = SampleCount::One;
	swapChainDesc.OldSwapChain = p->SwapChain.get();
	p->SwapChain = SwapChain::Create(swapChainDesc);

	if (!p->SwapChain)
		Utils::DisplayError(Utils::ErrorCode::VulkanSwapchainCreationFailed);

	//Create MSAA RenderTargets if needed
	if(p->CurrentAntiAliasing == AntiAliasing::MSAA)
	{
		RendererAPI::RenderTargetDesc rTMSAADesc{};
		rTMSAADesc.Width = window->GetFrameBufferSize().x();
		rTMSAADesc.Height = window->GetFrameBufferSize().y();
		rTMSAADesc.Depth = 1;
		rTMSAADesc.ArraySize = 1;
		rTMSAADesc.MipLevels = 1;
		rTMSAADesc.Format = SwapChain::GetRecommendedSwapchainFormat(true, false);
		rTMSAADesc.StartState = RendererAPI::ResourceState::RenderTarget;
		rTMSAADesc.SampleCount = p->CurrentSampleCount;
		for(u32 i = 0; i < RendererAPI::ImageCount; ++i)
			p->InternalRenderTargets[i] = RenderTarget::Create(rTMSAADesc);
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
	for(u32 i = 0; i < RendererAPI::ImageCount; ++i)
		p->RenderTargets[i] = RenderTarget::Create(rTDesc);
	if(p->CurrentAntiAliasing == AntiAliasing::MSAA)
	{
		rTDesc.SampleCount = p->CurrentSampleCount;
		for(u32 i = 0; i < RendererAPI::ImageCount; ++i)
			p->InternalRenderTargets[i] = RenderTarget::Create(rTDesc);
	}
#endif

	//Graphics Pipeline
#ifndef TRAP_HEADLESS_MODE
	TRAP::Ref<RenderTarget> rT = p->CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA ?
	                             p->InternalRenderTargets[0] :
								 p->SwapChain->GetRenderTargets()[0];
#else
	const auto& rT = p->CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA ? p->InternalRenderTargets[0] : std::get<0>(p->RenderTargets);
#endif
	p->GraphicsPipelineDesc = {};
	p->GraphicsPipelineDesc.Type = PipelineType::Graphics;
	p->GraphicsPipelineDesc.Pipeline = GraphicsPipelineDesc();
	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);

	gpd.PrimitiveTopology = PrimitiveTopology::TriangleList;
	gpd.RenderTargetCount = 1;
	gpd.ColorFormats = { rT->GetImageFormat() };
	gpd.SampleCount = rT->GetSampleCount();
	gpd.SampleQuality = rT->GetSampleQuality();
	gpd.ShadingRate = RendererAPI::ShadingRate::Full;
	gpd.ShadingRateCombiners = {ShadingRateCombiner::Passthrough, ShadingRateCombiner::Passthrough};

	gpd.DepthState = TRAP::MakeRef<DepthStateDesc>();
	gpd.DepthState->DepthTest = false;
	gpd.DepthState->DepthWrite = false;
	gpd.DepthState->DepthFunc = CompareMode::GreaterOrEqual;
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
	gpd.BlendState->Masks = {(BIT(0u) | BIT(1u) | BIT(2u) | BIT(3u))};
	gpd.BlendState->RenderTargetMask = BlendStateTargets::BlendStateTargetAll;
	gpd.BlendState->IndependentBlend = false;

	gpd.VertexLayout = TRAP::MakeRef<VertexLayout>();
	const TRAP::Ref<VertexLayout>& vLayout = gpd.VertexLayout;
	vLayout->AttributeCount = 1;
	std::get<0>(vLayout->Attributes).Format = ImageFormat::R32G32B32_SFLOAT;
	std::get<0>(vLayout->Attributes).Binding = 0;
	std::get<0>(vLayout->Attributes).Location = 0;
	std::get<0>(vLayout->Attributes).Offset = 0;

	//Compute Pipeline
	p->ComputePipelineDesc = {};
	p->ComputePipelineDesc.Type = PipelineType::Compute;
	p->ComputePipelineDesc.Pipeline = ComputePipelineDesc();
	p->CurrentComputeWorkGroupSize = {1u, 1u, 1u};


#ifndef TRAP_HEADLESS_MODE
	s_perViewportDataMap[window] = std::move(p);

	StartComputeRecording(s_perViewportDataMap.at(window).get());
	StartGraphicRecording(s_perViewportDataMap.at(window).get());
#else
	s_perViewportData = std::move(p);

	StartComputeRecording(s_perViewportData.get());
	StartGraphicRecording(s_perViewportData.get());
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::RemovePerViewportData(const Window* const window) const
#else
void TRAP::Graphics::API::VulkanRenderer::RemovePerViewportData() const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	s_perViewportDataMap.erase(window);
#else
	s_perViewportData.reset();
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::WaitIdle() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	m_device->WaitIdle();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<std::string> TRAP::Graphics::API::VulkanRenderer::SetupInstanceLayers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	std::vector<std::string> layers{};

#ifdef ENABLE_GRAPHICS_DEBUG
	//Turn on all validations
	if (VulkanInstance::IsLayerSupported("VK_LAYER_KHRONOS_validation"))
		layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif /*ENABLE_GRAPHICS_DEBUG*/

#ifdef USE_RENDER_DOC
	//Turn on render doc layer for gpu capture
	if(VulkanInstance::IsLayerSupported("VK_LAYER_RENDERDOC_Capture"))
		layers.emplace_back("VK_LAYER_RENDERDOC_Capture");
#endif /*USE_RENDER_DOC*/

	return layers;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<std::string> TRAP::Graphics::API::VulkanRenderer::SetupInstanceExtensions()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	std::vector<std::string> extensions{};

#ifndef TRAP_HEADLESS_MODE
	const auto reqExt = INTERNAL::WindowingAPI::GetRequiredInstanceExtensions();

	if(!VulkanInstance::IsExtensionSupported(std::get<0>(reqExt)) || !VulkanInstance::IsExtensionSupported(std::get<1>(reqExt)))
		Utils::DisplayError(Utils::ErrorCode::VulkanSurfaceExtensionsUnsupported);
	else
	{
		extensions.push_back(std::get<0>(reqExt));
		extensions.push_back(std::get<1>(reqExt));
	}
#endif /*TRAP_HEADLESS_MODE*/

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
#endif /*ENABLE_GPU_BASED_VALIDATION*/

#endif /*ENABLE_GRAPHICS_DEBUG*/

#ifndef TRAP_HEADLESS_MODE
	///HDR support (requires surface extension)
	if (VulkanInstance::IsExtensionSupported(VK_KHR_SURFACE_EXTENSION_NAME) &&
	    VulkanInstance::IsExtensionSupported(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
		s_swapchainColorSpaceExtension = true;
	}
#endif /*TRAP_HEADLESS_MODE*/

	return extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<std::string> TRAP::Graphics::API::VulkanRenderer::SetupDeviceExtensions(VulkanPhysicalDevice* const physicalDevice)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	std::vector<std::string> extensions{};

#ifndef TRAP_HEADLESS_MODE
	if(physicalDevice->IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
		extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	else
		Utils::DisplayError(Utils::ErrorCode::VulkanSwapchainExtensionsUnsupported);
#endif /*TRAP_HEADLESS_MODE*/

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
#endif /*ENABLE_DEBUG_UTILS_EXTENSION*/

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

	if (physicalDevice->IsExtensionSupported(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
		s_timelineSemaphore = true;
	}

	if (physicalDevice->IsExtensionSupported(VK_KHR_MULTIVIEW_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_MULTIVIEW_EXTENSION_NAME);
		s_multiView = true;
	}

	if (s_multiView && physicalDevice->IsExtensionSupported(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
		s_renderPass2 = true;
	}

	if(physicalDevice->IsExtensionSupported(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME))
		extensions.emplace_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);

	if(physicalDevice->IsExtensionSupported(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME) &&
	   physicalDevice->IsExtensionSupported(VK_KHR_SPIRV_1_4_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
		s_SPIRV1_4 = true;
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
#endif /*TRAP_PLATFORM_WINDOWS*/

	//RayTracing
	if(s_descriptorIndexingExtension &&
	   physicalDevice->IsExtensionSupported(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME) &&
	   s_bufferDeviceAddressExtension &&
	   s_SPIRV1_4 &&
	   physicalDevice->IsExtensionSupported(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME) &&
	   physicalDevice->IsExtensionSupported(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME) &&
	   physicalDevice->IsExtensionSupported(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) &&
	   physicalDevice->IsExtensionSupported(VK_KHR_RAY_QUERY_EXTENSION_NAME))
	{
		extensions.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
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
#endif /*ENABLE_NSIGHT_AFTERMATH*/

	return extensions;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::AddDefaultResources()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPrefix, "Creating DefaultResources");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	s_NullDescriptors = TRAP::MakeScope<NullDescriptors>();

	TRAP::Ref<TRAP::Graphics::API::VulkanTexture> vkTex = nullptr;

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
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim1D)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim1D)] = std::move(vkTex);

	//1D Texture Array
	textureDesc.ArraySize = 2;
	textureDesc.Descriptors = DescriptorType::Texture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim1DArray)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim1DArray)] = std::move(vkTex);

	//2D Texture
	textureDesc.Width = 2;
	textureDesc.Height = 2;
	textureDesc.ArraySize = 1;
	textureDesc.Descriptors = DescriptorType::Texture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2D)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2D)] = std::move(vkTex);

	//2D MS Texture
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.SampleCount = SampleCount::Four;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2DMS)] = std::move(vkTex);
	textureDesc.SampleCount = SampleCount::One;

	//2D Texture Array
	textureDesc.ArraySize = 2;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2DArray)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2DArray)] = std::move(vkTex);

	//2D MS Texture Array
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.SampleCount = SampleCount::Four;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2DMSArray)] = std::move(vkTex);
	textureDesc.SampleCount = SampleCount::One;

	//3D Texture
	textureDesc.Depth = 2;
	textureDesc.ArraySize = 1;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim3D)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim3D)] = std::move(vkTex);

	//Cube Texture
	textureDesc.Depth = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Descriptors = DescriptorType::TextureCube;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDimCube)] = std::move(vkTex);
	textureDesc.ArraySize = 6 * 2;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>();
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDimCubeArray)] = std::move(vkTex);

	BufferDesc bufferDesc{};
	bufferDesc.Descriptors = DescriptorType::Buffer | DescriptorType::UniformBuffer;
	bufferDesc.MemoryUsage = ResourceMemoryUsage::GPUOnly;
	bufferDesc.StartState = ResourceState::Common;
	bufferDesc.Size = sizeof(u32);
	bufferDesc.FirstElement = 0;
	bufferDesc.ElementCount = 1;
	bufferDesc.StructStride = sizeof(u32);
	bufferDesc.Format = ImageFormat::R32_UINT;
	s_NullDescriptors->DefaultBufferSRV = TRAP::MakeRef<VulkanBuffer>(bufferDesc);
	bufferDesc.Descriptors = DescriptorType::RWBuffer;
	s_NullDescriptors->DefaultBufferUAV = TRAP::MakeRef<VulkanBuffer>(bufferDesc);

	SamplerDesc samplerDesc{};
	samplerDesc.AddressU = AddressMode::ClampToBorder;
	samplerDesc.AddressV = AddressMode::ClampToBorder;
	samplerDesc.AddressW = AddressMode::ClampToBorder;
	samplerDesc.EnableAnisotropy = false;
	s_NullDescriptors->DefaultSampler = TRAP::MakeRef<VulkanSampler>(samplerDesc);

	BlendStateDesc blendStateDesc{};
	std::get<0>(blendStateDesc.DstAlphaFactors) = BlendConstant::Zero;
	std::get<0>(blendStateDesc.DstFactors) = BlendConstant::Zero;
	std::get<0>(blendStateDesc.SrcAlphaFactors) = BlendConstant::One;
	std::get<0>(blendStateDesc.SrcFactors) = BlendConstant::One;
	std::get<0>(blendStateDesc.Masks) = (BIT(0u) | BIT(1u) | BIT(2u) | BIT(3u));
	blendStateDesc.RenderTargetMask = BlendStateTargets::BlendStateTargetAll;
	blendStateDesc.IndependentBlend = false;
	DefaultBlendDesc = UtilToBlendDesc(blendStateDesc, DefaultBlendAttachments);

	DepthStateDesc depthStateDesc{};
	depthStateDesc.DepthFunc = CompareMode::GreaterOrEqual;
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
	const TRAP::Ref<VulkanQueue> graphicsQueue = TRAP::MakeRef<VulkanQueue>(queueDesc);

	CommandPoolDesc cmdPoolDesc{};
	cmdPoolDesc.Queue = graphicsQueue;
	cmdPoolDesc.Transient = true;
	const TRAP::Ref<VulkanCommandPool> cmdPool = TRAP::MakeRef<VulkanCommandPool>(cmdPoolDesc);

	CommandBuffer* const cmd = cmdPool->AllocateCommandBuffer(false);

	const TRAP::Ref<VulkanFence> fence = TRAP::MakeRef<VulkanFence>();

	s_NullDescriptors->InitialTransitionQueue = graphicsQueue;
	s_NullDescriptors->InitialTransitionCmdPool = cmdPool;
	s_NullDescriptors->InitialTransitionCmd = dynamic_cast<VulkanCommandBuffer*>(cmd);
	s_NullDescriptors->InitialTransitionFence = fence;

	//Transition resources
	for (u32 dim = 0; dim < std::to_underlying(ShaderReflection::TextureDimension::TextureDimCount); ++dim)
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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(!s_NullDescriptors)
		return;

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPrefix, "Destroying DefaultResources");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	for(u32 dim = 0; dim < std::to_underlying(ShaderReflection::TextureDimension::TextureDimCount); ++dim)
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

void TRAP::Graphics::API::VulkanRenderer::UtilInitialTransition(const Ref<TRAP::Graphics::Texture>& texture,
                                                                const RendererAPI::ResourceState startState)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	std::lock_guard lock(s_NullDescriptors->InitialTransitionMutex);
	auto& mutex = s_NullDescriptors->InitialTransitionMutex;
	LockMark(mutex);
	VulkanCommandBuffer* const cmd = s_NullDescriptors->InitialTransitionCmd;
	s_NullDescriptors->InitialTransitionCmdPool->Reset();
	cmd->Begin();
	const TextureBarrier barrier{texture.get(), RendererAPI::ResourceState::Undefined, startState};
	cmd->ResourceBarrier(nullptr, &barrier, nullptr);
	cmd->End();
	RendererAPI::QueueSubmitDesc submitDesc{};
	submitDesc.Cmds = {cmd};
	submitDesc.SignalFence = s_NullDescriptors->InitialTransitionFence;
	s_NullDescriptors->InitialTransitionQueue->Submit(submitDesc);
	s_NullDescriptors->InitialTransitionFence->Wait();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::VulkanRenderer::RenderPassMap& TRAP::Graphics::API::VulkanRenderer::GetRenderPassMap()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	//Only need a lock when creating a new RenderPass Map for this thread
	std::lock_guard lock(s_renderPassMutex);
	LockMark(s_renderPassMutex);
	const auto it = s_renderPassMap.find(std::this_thread::get_id());
	if (it == s_renderPassMap.end())
	{
		s_renderPassMap[std::this_thread::get_id()] = {};
		return s_renderPassMap[std::this_thread::get_id()];
	}

	return it->second;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::VulkanRenderer::FrameBufferMap& TRAP::Graphics::API::VulkanRenderer::GetFrameBufferMap()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	//Only need a lock when creating a new FrameBuffer Map for this thread
	std::lock_guard lock(s_renderPassMutex);
	LockMark(s_renderPassMutex);
	const auto it = s_frameBufferMap.find(std::this_thread::get_id());
	if(it == s_frameBufferMap.end())
	{
		s_frameBufferMap[std::this_thread::get_id()] = {};
		return s_frameBufferMap[std::this_thread::get_id()];
	}

	return it->second;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanInstance> TRAP::Graphics::API::VulkanRenderer::GetInstance() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_instance;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanDevice> TRAP::Graphics::API::VulkanRenderer::GetDevice() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_device;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanMemoryAllocator> TRAP::Graphics::API::VulkanRenderer::GetVMA() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_vma;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Ref<TRAP::Graphics::Pipeline>& TRAP::Graphics::API::VulkanRenderer::GetPipeline(PipelineDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const usize hash = std::hash<PipelineDesc>{}(desc);
	const auto pipelineIt = s_pipelines.find(hash);

	if(pipelineIt != s_pipelines.end())
		return pipelineIt->second;

	const auto tempFolder = TRAP::FileSystem::GetGameTempFolderPath();
	if(tempFolder)
	{
		std::pair<std::unordered_map<u64, TRAP::Ref<PipelineCache>>::iterator, bool> res;
		if (!s_pipelineCaches.contains(hash))
		{
			PipelineCacheLoadDesc cacheDesc{};
			cacheDesc.Path = *tempFolder / fmt::format("{}.cache", hash);
			res = s_pipelineCaches.try_emplace(hash, PipelineCache::Create(cacheDesc));
		}

		if(res.second) //Got inserted
			desc.Cache = res.first->second;
	}

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_TRACE(Log::RendererVulkanPipelinePrefix, "Recreating Graphics Pipeline...");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	const TRAP::Ref<TRAP::Graphics::Pipeline> pipeline = Pipeline::Create(desc);
	const auto pipeRes = s_pipelines.try_emplace(hash, pipeline);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_TRACE(Log::RendererVulkanPipelinePrefix, "Cached Graphics Pipeline");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	return pipeRes.first->second;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BeginGPUFrameProfile(const QueueType type, const PerViewportData* const p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const CommandBuffer* cmd = nullptr;
	if(type == QueueType::Graphics)
		cmd = p->GraphicCommandBuffers[p->ImageIndex];
	else if(type == QueueType::Compute)
		cmd = p->ComputeCommandBuffers[p->ImageIndex];

	TRAP::Ref<QueryPool> pool = nullptr;
	if(type == QueueType::Graphics)
		pool = p->GraphicsTimestampQueryPools[p->ImageIndex];
	else if(type == QueueType::Compute)
		pool = p->ComputeTimestampQueryPools[p->ImageIndex];

	cmd->ResetQueryPool(pool, 0, 1 * 2);
	cmd->BeginQuery(pool, {0});
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::EndGPUFrameProfile(const QueueType type, const PerViewportData* const p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const CommandBuffer* cmd = nullptr;
	if(type == QueueType::Graphics)
		cmd = p->GraphicCommandBuffers[p->ImageIndex];
	else if(type == QueueType::Compute)
		cmd = p->ComputeCommandBuffers[p->ImageIndex];

	TRAP::Ref<QueryPool> pool = nullptr;
	if(type == QueueType::Graphics)
		pool = p->GraphicsTimestampQueryPools[p->ImageIndex];
	else if(type == QueueType::Compute)
		pool = p->ComputeTimestampQueryPools[p->ImageIndex];

	TRAP::Ref<Buffer> buffer = nullptr;
	if(type == QueueType::Graphics)
		buffer = p->GraphicsTimestampReadbackBuffers[p->ImageIndex];
	else if(type == QueueType::Compute)
		buffer = p->ComputeTimestampReadbackBuffers[p->ImageIndex];

	cmd->BeginQuery(pool, {1});
	cmd->ResolveQuery(pool, buffer, 0, 1 * 2);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Graphics::API::VulkanRenderer::ResolveGPUFrameProfile(const QueueType type, const PerViewportData* const p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	f32 time = 0.0f;

	RendererAPI::ReadRange readRange{0, sizeof(u64) * 1 * 2};
	TRAP::Ref<Buffer> buffer = nullptr;
	if(type == QueueType::Graphics)
		buffer = p->GraphicsTimestampReadbackBuffers[p->ImageIndex];
	else if(type == QueueType::Compute)
		buffer = p->ComputeTimestampReadbackBuffers[p->ImageIndex];
	buffer->MapBuffer(&readRange);
	if(buffer->GetCPUMappedAddress() != nullptr)
	{
		const u64 startTime = *(static_cast<u64*>(buffer->GetCPUMappedAddress()) + 0);
		const u64 endTime = *(static_cast<u64*>(buffer->GetCPUMappedAddress()) + 1);

		if(endTime > startTime)
		{
			const f64 nsTime = NumericCast<f64>(endTime - startTime);
			if(type == QueueType::Graphics)
			{
				const Ref<VulkanQueue> graphicsQueue = std::dynamic_pointer_cast<VulkanQueue>(s_graphicQueue);
				time = NumericCast<f32>(nsTime / graphicsQueue->GetTimestampFrequency()) * 1000.0f;
			}
			else if(type == QueueType::Compute)
			{
				const Ref<VulkanQueue> computeQueue = std::dynamic_pointer_cast<VulkanQueue>(s_computeQueue);
				time = NumericCast<f32>(nsTime / computeQueue->GetTimestampFrequency()) * 1000.0f;
			}
		}

		buffer->UnMapBuffer();
	}

	return time;
}