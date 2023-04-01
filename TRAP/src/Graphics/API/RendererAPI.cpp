#include "TRAPPCH.h"
#include "RendererAPI.h"

#include "Application.h"
#include "Vulkan/VulkanRenderer.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Vulkan/VulkanCommon.h"
#include "Vulkan/Objects/VulkanInits.h"
#include "Vulkan/Objects/VulkanInstance.h"
#include "Vulkan/Objects/VulkanPhysicalDevice.h"
#include "ResourceLoader.h"
#include "Objects/CommandPool.h"
#include "Objects/CommandBuffer.h"
#include "Objects/Queue.h"
#include "Objects/Sampler.h"
#include "Objects/SwapChain.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::RendererAPI> TRAP::Graphics::RendererAPI::s_Renderer = nullptr;
TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::s_RenderAPI = TRAP::Graphics::RenderAPI::NONE;
TRAP::Scope<TRAP::Graphics::API::ResourceLoader> TRAP::Graphics::RendererAPI::s_ResourceLoader = nullptr;
std::unordered_map<const TRAP::Window*,
                   TRAP::Scope<TRAP::Graphics::RendererAPI::PerWindowData>> TRAP::Graphics::RendererAPI::s_perWindowDataMap = {};
bool TRAP::Graphics::RendererAPI::s_isVulkanCapable = true;
bool TRAP::Graphics::RendererAPI::s_isVulkanCapableFirstTest = true;
TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::RendererAPI::s_descriptorPool = nullptr;
TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::s_graphicQueue = nullptr;
TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::s_computeQueue = nullptr;
TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::s_transferQueue = nullptr;

TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::RendererAPI::s_currentSampleCount = TRAP::Graphics::RendererAPI::SampleCount::Two;
TRAP::Graphics::RendererAPI::AntiAliasing TRAP::Graphics::RendererAPI::s_currentAntiAliasing = TRAP::Graphics::RendererAPI::AntiAliasing::Off;
TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::RendererAPI::s_newSampleCount = TRAP::Graphics::RendererAPI::SampleCount::Two;
TRAP::Graphics::RendererAPI::AntiAliasing TRAP::Graphics::RendererAPI::s_newAntiAliasing = TRAP::Graphics::RendererAPI::AntiAliasing::Off;
TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::RendererAPI::s_Anisotropy = TRAP::Graphics::RendererAPI::SampleCount::Sixteen;

std::array<uint8_t, 16> TRAP::Graphics::RendererAPI::s_newGPUUUID{};

#ifdef ENABLE_NSIGHT_AFTERMATH
bool TRAP::Graphics::RendererAPI::s_aftermathSupport = false;
bool TRAP::Graphics::RendererAPI::s_diagnosticsConfigSupport = false;
bool TRAP::Graphics::RendererAPI::s_diagnosticCheckPointsSupport = false;
#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Init(const std::string_view gameName, const RenderAPI renderAPI)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(s_Renderer)
		return;

	s_RenderAPI = renderAPI;

	switch (s_RenderAPI)
	{
	case RenderAPI::Vulkan:
		TP_INFO(Log::RendererVulkanPrefix, "Initializing Renderer");
		s_Renderer = MakeScope<API::VulkanRenderer>();
		break;

	default:
		Utils::Dialogs::ShowMsgBox("Unsupported device", "Device is unsupported!\nNo RenderAPI selected!\n"
								   "Error code: 0x0002", Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::RendererPrefix, "Unsupported device! (0x0002)");
		exit(0x0002);
	}

	s_Renderer->InitInternal(gameName);

	//Create Graphic Queue
	QueueDesc queueDesc{};
	queueDesc.Type = QueueType::Graphics;
	s_graphicQueue = Queue::Create(queueDesc);

	//Create Compute Queue
	queueDesc.Type = QueueType::Compute;
	s_computeQueue = Queue::Create(queueDesc);

	//Create Transfer Queue
	queueDesc.Type = QueueType::Transfer;
	s_transferQueue = Queue::Create(queueDesc);

	s_Anisotropy = static_cast<SampleCount>(GPUSettings.MaxAnisotropy);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	s_perWindowDataMap.clear();

	TRAP::Graphics::Sampler::ClearCache();

	s_Renderer->s_graphicQueue->WaitQueueIdle();
	s_Renderer->s_computeQueue->WaitQueueIdle();
	s_Renderer->s_transferQueue->WaitQueueIdle();
	s_Renderer->s_graphicQueue.reset();
	s_Renderer->s_computeQueue.reset();
	s_Renderer->s_transferQueue.reset();

	s_Renderer.reset();
	s_Renderer = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI* TRAP::Graphics::RendererAPI::GetRenderer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

#ifdef TRAP_HEADLESS_MODE
	TRAP_ASSERT(s_RenderAPI != RenderAPI::NONE , "RendererAPI::GetRenderer(): RendererAPI is not available because RenderAPI::NONE is set (or EnableGPU=False)!");
#endif

	return s_Renderer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ResourceLoader* TRAP::Graphics::RendererAPI::GetResourceLoader() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_ResourceLoader.get();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::AutoSelectRenderAPI()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TP_INFO(Log::RendererPrefix, "Auto selecting RenderAPI");

	//Check if Vulkan capable
	if (s_Renderer->IsVulkanCapable())
		return RenderAPI::Vulkan;
	TP_WARN(Log::RendererVulkanPrefix, "Device isn't Vulkan 1.1 capable!");


#ifndef TRAP_HEADLESS_MODE
	s_RenderAPI = RenderAPI::NONE;
	TRAP::Utils::Dialogs::ShowMsgBox("Incompatible device (GPU)",
		                             "TRAP™ was unable to detect a compatible RenderAPI!\n"
									 "Does your system meet the minimum system requirements for running TRAP™?\n"
									 "Please check your GPU driver!\nError code: 0x000B", Utils::Dialogs::Style::Error,
		Utils::Dialogs::Buttons::Quit);
	TP_CRITICAL(Log::RendererPrefix, "TRAP™ was unable to detect a compatible RenderAPI! (0x000B)");
	exit(0x000B);
#else
	TP_WARN(Log::RendererPrefix, "Disabling RendererAPI, no compatible RenderAPI was found!");
	return RenderAPI::NONE;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::RendererAPI::IsSupported(const RenderAPI api)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if (api == RenderAPI::Vulkan)
		return s_Renderer->IsVulkanCapable();

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::GetRenderAPI() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_RenderAPI;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::SetNewGPU(std::array<uint8_t, 16> GPUUUID) noexcept
{
	s_newGPUUUID = std::move(GPUUUID);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<uint8_t, 16> TRAP::Graphics::RendererAPI::GetNewGPU() noexcept
{
	return s_newGPUUUID;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::OnPostUpdate()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	for(const auto& [win, data] : s_perWindowDataMap)
	{
		data->State = PerWindowState::PostUpdate;

		if(data->RenderScale != 1.0f)
		{
			TRAP::Ref<RenderTarget> outputTarget = nullptr;
#ifndef TRAP_HEADLESS_MODE
			outputTarget = data->SwapChain->GetRenderTargets()[data->CurrentSwapChainImageIndex];
#else
			outputTarget = data->RenderTargets[data->CurrentSwapChainImageIndex];
#endif

			GetRenderer()->RenderScalePass(data->InternalRenderTargets[data->CurrentSwapChainImageIndex],
			                               outputTarget, win);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::RendererAPI::GetDescriptorPool() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_descriptorPool;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::GetGraphicsQueue() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_graphicQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::GetComputeQueue() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_computeQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::GetTransferQueue() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_transferQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::PerWindowData& TRAP::Graphics::RendererAPI::GetWindowData(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (!window)
		window = TRAP::Application::GetWindow();

	return *s_perWindowDataMap.at(window);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::RendererAPI::GetGraphicsRootSignature(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (!window)
		window = TRAP::Application::GetWindow();

	return std::get<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>
	(
		s_perWindowDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	).RootSignature;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2ui TRAP::Graphics::RendererAPI::GetInternalRenderResolution(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (!window)
		window = TRAP::Application::GetWindow();

	const float renderScale = s_perWindowDataMap.at(window)->RenderScale;

#ifdef TRAP_HEADLESS_MODE
	const auto& winData = s_perWindowDataMap.at(window);
#endif

	if(renderScale == 1.0f)
	{
#ifndef TRAP_HEADLESS_MODE
		return window->GetFrameBufferSize();
#else
		return {winData->NewWidth, winData->NewHeight};
#endif
	}

	const Math::Vec2 frameBufferSize
	{
#ifndef TRAP_HEADLESS_MODE
		static_cast<float>(window->GetFrameBufferSize().x),
		static_cast<float>(window->GetFrameBufferSize().y)
#else
		static_cast<float>(winData->NewWidth),
		static_cast<float>(winData->NewHeight),
#endif
	};
	const float aspectRatio = frameBufferSize.x / frameBufferSize.y;

	Math::Vec2 finalRes = frameBufferSize * renderScale;

	//Make sure the resolution is an integer scale of the framebuffer size.
	//This is done to avoid scaling artifacts (like blurriness).
	while((finalRes.x / finalRes.y) != aspectRatio)
	{
		if((finalRes.x / finalRes.y) <= aspectRatio)
			++finalRes.x;
		else
			++finalRes.y;
	}

	return static_cast<Math::Vec2ui>(finalRes);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::StartRenderPass(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!window)
		window = TRAP::Application::GetWindow();

	const auto* const winData = s_perWindowDataMap.at(window).get();

	TRAP::Ref<Graphics::RenderTarget> renderTarget = nullptr;
#ifndef TRAP_HEADLESS_MODE
	//Get correct RenderTarget
	if((winData->RenderScale != 1.0f || s_currentAntiAliasing == RendererAPI::AntiAliasing::MSAA) && winData->State == PerWindowState::PreUpdate)
		renderTarget = winData->InternalRenderTargets[winData->CurrentSwapChainImageIndex];
	else
		renderTarget = winData->SwapChain->GetRenderTargets()[winData->CurrentSwapChainImageIndex];

	GetRenderer()->BindRenderTarget(renderTarget, nullptr, nullptr,
									nullptr, nullptr, static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), window);
#else
	if((winData->RenderScale != 1.0f || s_currentAntiAliasing == RendererAPI::AntiAliasing::MSAA) && winData->State == PerWindowState::PreUpdate)
		renderTarget = winData->InternalRenderTargets[winData->ImageIndex];
	else
		renderTarget = winData->RenderTargets[winData->ImageIndex];

	GetRenderer()->BindRenderTarget(renderTarget, nullptr, nullptr,
	                                nullptr, nullptr, static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), window);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::StopRenderPass(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!window)
		window = TRAP::Application::GetWindow();

	GetRenderer()->BindRenderTarget(nullptr, nullptr, nullptr, nullptr, nullptr, static_cast<uint32_t>(-1),
	                                static_cast<uint32_t>(-1), window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Transition(Ref<TRAP::Graphics::Texture> texture,
											 const TRAP::Graphics::RendererAPI::ResourceState oldLayout,
											 const TRAP::Graphics::RendererAPI::ResourceState newLayout,
											 const TRAP::Graphics::RendererAPI::QueueType queueType)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute ||
	            queueType == QueueType::Transfer, "RendererAPI::Transition(): Invalid queue type provided!");

	TRAP::Ref<TRAP::Graphics::Queue> queue = nullptr;
	if(queueType == QueueType::Graphics)
		queue = s_graphicQueue;
	else if(queueType == QueueType::Compute)
		queue = s_computeQueue;
	else if(queueType == QueueType::Transfer)
		queue = s_transferQueue;

	CommandPoolDesc cmdPoolDesc{};
	cmdPoolDesc.Queue = queue;
	cmdPoolDesc.Transient = true;
	TRAP::Ref<CommandPool> cmdPool = TRAP::Graphics::CommandPool::Create(cmdPoolDesc);

	CommandBuffer* const cmd = cmdPool->AllocateCommandBuffer(false);

	//Start recording
	cmd->Begin();

	//Transition the texture to the correct state
	TextureBarrier texBarrier{};
	texBarrier.Texture = texture.get();
	texBarrier.CurrentState = oldLayout;
	texBarrier.NewState = newLayout;

	cmd->ResourceBarrier(nullptr, &texBarrier, nullptr);

	//End recording
	cmd->End();

	//Submit the command buffer
	QueueSubmitDesc submitDesc{};
	submitDesc.Cmds = {cmd};

	queue->Submit(submitDesc);

	//Wait for work to finish on the GPU
	queue->WaitQueueIdle();

	//Cleanup
	cmdPool->FreeCommandBuffer(cmd);
	cmdPool.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	outAntiAliasing = s_currentAntiAliasing;
	outSampleCount = s_currentSampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::SetAntiAliasing(const AntiAliasing antiAliasing, SampleCount sampleCount)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(GPUSettings.MaxMSAASampleCount >= sampleCount, "RendererAPI::SetAntiAliasing(): Sample count is higher than max supported by GPU");

	if(antiAliasing == AntiAliasing::MSAA && sampleCount > GPUSettings.MaxMSAASampleCount)
		sampleCount = GPUSettings.MaxMSAASampleCount;
	else if(antiAliasing != AntiAliasing::Off && sampleCount == SampleCount::One)
	{
		TRAP_ASSERT(false, "RendererAPI::SetAntiAliasing(): Sample count must be greater than one when anti aliasing is enabled");
		sampleCount = SampleCount::Two; //Fail safe
	}
	else if(antiAliasing == AntiAliasing::Off)
		sampleCount = SampleCount::Two;

	s_newAntiAliasing = antiAliasing;
	s_newSampleCount = sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::RendererAPI::GetAnisotropyLevel() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Anisotropy;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::SetAnisotropyLevel(const SampleCount anisotropyLevel)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(GPUSettings.MaxAnisotropy >= static_cast<float>(anisotropyLevel), "RendererAPI::SetAnisotropyLevel(): Anisotropy level is higher than max supported by GPU");

	s_Anisotropy = static_cast<SampleCount>(TRAP::Math::Clamp(static_cast<float>(anisotropyLevel), 1.0f, GPUSettings.MaxAnisotropy));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::ResizeSwapChain(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (!window)
		window = TRAP::Application::GetWindow();

	s_perWindowDataMap.at(window)->ResizeSwapChain = true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Graphics::RendererAPI::GetGPUGraphicsFrameTime(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(!window)
		window = TRAP::Application::GetWindow();

	return s_perWindowDataMap.at(window)->GraphicsFrameTime;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Graphics::RendererAPI::GetGPUComputeFrameTime(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(!window)
		window = TRAP::Application::GetWindow();

	return s_perWindowDataMap.at(window)->ComputeFrameTime;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::RendererAPI::IsVulkanCapable()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!s_isVulkanCapableFirstTest)
		return s_isVulkanCapable;

	TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
	TP_INFO(Log::RendererVulkanPrefix, "Running Vulkan capability tester");

	s_isVulkanCapableFirstTest = false;

	if(TRAP::Utils::GetLinuxWindowManager() != TRAP::Utils::LinuxWindowManager::Unknown)
	{
		if (!INTERNAL::WindowingAPI::Init())
		{
			Utils::Dialogs::ShowMsgBox("Failed to initialize WindowingAPI", "The WindowingAPI couldn't be initialized!\n"
								       "Error code: 0x0011", Utils::Dialogs::Style::Error,
								       Utils::Dialogs::Buttons::Quit);
			TP_CRITICAL(Log::RendererVulkanPrefix, "Failed to initialize WindowingAPI! (0x0011)");
			exit(0x0011);
		}
	}

	if (INTERNAL::WindowingAPI::VulkanSupported())
	{
		if(VkGetInstanceVersion() < VK_API_VERSION_1_1)
		{
			TP_CRITICAL(Log::RendererVulkanPrefix, "Failed instance version test!");
			TP_CRITICAL(Log::RendererVulkanPrefix, "Failed Vulkan capability tester!");
			TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
			s_isVulkanCapable = false;
			return s_isVulkanCapable;
		}

		//Required: Instance Extensions
		//Surface extensions are optional in Headless mode.
		std::vector<std::string> instanceExtensions{};
		const auto reqExt = INTERNAL::WindowingAPI::GetRequiredInstanceExtensions();

		if (std::get<0>(reqExt).empty() ||
		    std::get<1>(reqExt).empty() ||
			!API::VulkanInstance::IsExtensionSupported(std::get<0>(reqExt)) ||
			!API::VulkanInstance::IsExtensionSupported(std::get<1>(reqExt)))
		{
			GPUSettings.SurfaceSupported = false;
			TP_WARN(Log::RendererVulkanPrefix, "Failed required instance extension test");
			TP_WARN(Log::RendererVulkanPrefix, "Failed Vulkan capability tester!");
			TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
			s_isVulkanCapable = false;
			return s_isVulkanCapable;
		}
		else
		{
			GPUSettings.SurfaceSupported = true;
			instanceExtensions.push_back(std::get<0>(reqExt));
			instanceExtensions.push_back(std::get<1>(reqExt));
		}

		if(!API::VulkanInstance::IsExtensionSupported(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
		{
			TP_WARN(Log::RendererVulkanPrefix, "Failed required instance extension test");
			TP_WARN(Log::RendererVulkanPrefix, "Failed Vulkan capability tester!");
			TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
			s_isVulkanCapable = false;
			return s_isVulkanCapable;
		}
		else
			instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

		//Create Instance
		VkInstance instance = VK_NULL_HANDLE;
		std::vector<const char*> extensions(instanceExtensions.size());
		for (uint32_t i = 0; i < instanceExtensions.size(); i++)
			extensions[i] = instanceExtensions[i].c_str();
		const VkApplicationInfo appInfo = API::VulkanInits::ApplicationInfo("Vulkan Capability Tester");
		const VkInstanceCreateInfo instanceCreateInfo = API::VulkanInits::InstanceCreateInfo(appInfo, {}, extensions);
		const VkResult res = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
		if (res == VK_SUCCESS && instance)
		{
			VkLoadInstance(instance);

			//Physical Device
			const auto& physicalDevices = API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(instance);
			if(physicalDevices.empty())
			{
				s_isVulkanCapable = false;
				TP_WARN(Log::RendererVulkanPrefix,
						"Failed to find a suitable GPU meeting all requirements!");
			}
		}
		else
		{
			VkCall(res);
			s_isVulkanCapable = false;
			TP_ERROR(Log::RendererVulkanPrefix, "Failed to create Vulkan instance!");
		}
	}
	else
		s_isVulkanCapable = false;

	if (s_isVulkanCapable)
	{
		TP_INFO(Log::RendererVulkanPrefix, "Passed Vulkan capability tester!");
		TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
	}
	else
	{
		TP_WARN(Log::RendererVulkanPrefix, "Failed Vulkan capability tester!");
		TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
	}

	return s_isVulkanCapable;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::PerWindowData::~PerWindowData()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SwapChain.reset();
	ImageAcquiredSemaphore.reset();

	for(int32_t i = ImageCount - 1; i >= 0; i--)
	{
#ifdef TRAP_HEADLESS_MODE
		RenderTargets[i].reset();
#endif

		RenderCompleteSemaphores[i].reset();
		RenderCompleteFences[i].reset();

		GraphicCommandPools[i]->FreeCommandBuffer(GraphicCommandBuffers[i]);
		GraphicCommandBuffers[i] = nullptr;
		GraphicCommandPools[i].reset();

		GraphicsTimestampQueryPools[i].reset();
		GraphicsTimestampReadbackBuffers[i].reset();

		ComputeCompleteSemaphores[i].reset();
		ComputeCompleteFences[i].reset();

		ComputeCommandPools[i]->FreeCommandBuffer(ComputeCommandBuffers[i]);
		ComputeCommandBuffers[i] = nullptr;
		ComputeCommandPools[i].reset();

		ComputeTimestampQueryPools[i].reset();
		ComputeTimestampReadbackBuffers[i].reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::RendererAPI::GetCurrentImageIndex(const TRAP::Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "RendererAPI::GetCurrentImageIndex(): Window is nullptr!");

	return s_perWindowDataMap.at(window)->ImageIndex;
}
