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
#include "ImageLoader/Image.h"
#include "Utils/ErrorCodes/ErrorCodes.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::RendererAPI> TRAP::Graphics::RendererAPI::s_Renderer = nullptr;
TRAP::Scope<TRAP::Graphics::API::ResourceLoader> TRAP::Graphics::RendererAPI::s_ResourceLoader = nullptr;
#ifndef TRAP_HEADLESS_MODE
std::unordered_map<const TRAP::Window*,
                   TRAP::Scope<TRAP::Graphics::RendererAPI::PerViewportData>> TRAP::Graphics::RendererAPI::s_perViewportDataMap = {};
#else
TRAP::Scope<TRAP::Graphics::RendererAPI::PerViewportData> TRAP::Graphics::RendererAPI::s_perViewportData = nullptr;
#endif /*TRAP_HEADLESS_MODE*/
TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::RendererAPI::s_descriptorPool = nullptr;
TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::s_graphicQueue = nullptr;
TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::s_computeQueue = nullptr;
TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::s_transferQueue = nullptr;

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
		Utils::DisplayError(Utils::ErrorCode::RenderAPIInvalid);
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

#ifndef TRAP_HEADLESS_MODE
	s_perViewportDataMap.clear();
#else
	s_perViewportData.reset();
#endif /*TRAP_HEADLESS_MODE*/

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

	TRAP_ASSERT(s_RenderAPI != RenderAPI::NONE , "RendererAPI::GetRenderer(): RendererAPI is not available because RenderAPI::NONE is set (or EnableGPU=False)!");

	return s_Renderer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ResourceLoader* TRAP::Graphics::RendererAPI::GetResourceLoader() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_RenderAPI != RenderAPI::NONE , "RendererAPI::GetResourceLoader(): ResourceLoader is not available because RenderAPI::NONE is set (or EnableGPU=False)!");

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
	Utils::DisplayError(Utils::ErrorCode::RenderAPINoSupportedFound);
	return RenderAPI::NONE;
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

void TRAP::Graphics::RendererAPI::SetNewGPU(const std::array<uint8_t, 16>& GPUUUID) noexcept
{
	s_newGPUUUID = GPUUUID;
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

#ifndef TRAP_HEADLESS_MODE
	for(const auto& [win, data] : s_perViewportDataMap)
	{
		data->State = PerWindowState::PostUpdate;

		if(data->RenderScale != 1.0f)
		{
			TRAP::Ref<RenderTarget> outputTarget = data->SwapChain->GetRenderTargets()[data->CurrentSwapChainImageIndex];

			GetRenderer()->RenderScalePass(data->InternalRenderTargets[data->CurrentSwapChainImageIndex],
			                               outputTarget, win);
		}
	}
#else
	s_perViewportData->State = PerWindowState::PostUpdate;

	if(s_perViewportData->RenderScale != 1.0f)
	{
		TRAP::Ref<RenderTarget> outputTarget = s_perViewportData->RenderTargets[s_perViewportData->CurrentSwapChainImageIndex];

		GetRenderer()->RenderScalePass(s_perViewportData->InternalRenderTargets[s_perViewportData->CurrentSwapChainImageIndex],
										outputTarget);
	}
#endif /*TRAP_HEADLESS_MODE*/
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

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Graphics::RendererAPI::PerViewportData& TRAP::Graphics::RendererAPI::GetViewportData(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (window == nullptr)
		window = TRAP::Application::GetWindow();

	return *s_perViewportDataMap.at(window);
}
#else
[[nodiscard]] TRAP::Graphics::RendererAPI::PerViewportData& TRAP::Graphics::RendererAPI::GetViewportData()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return *s_perViewportData;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::RendererAPI::GetGraphicsRootSignature(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (window == nullptr)
		window = TRAP::Application::GetWindow();

	return std::get<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>
	(
		s_perViewportDataMap.at(window)->GraphicsPipelineDesc.Pipeline
	).RootSignature;
}
#else
[[nodiscard]] TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::RendererAPI::GetGraphicsRootSignature()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return std::get<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>
	(
		s_perViewportData->GraphicsPipelineDesc.Pipeline
	).RootSignature;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Math::Vec2ui TRAP::Graphics::RendererAPI::GetInternalRenderResolution(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (window == nullptr)
		window = TRAP::Application::GetWindow();

	const float renderScale = s_perViewportDataMap.at(window)->RenderScale;

	if(renderScale == 1.0f)
		return window->GetFrameBufferSize();

	const Math::Vec2 frameBufferSize
	{
		NumericCast<float>(window->GetFrameBufferSize().x),
		NumericCast<float>(window->GetFrameBufferSize().y)
	};
	const float aspectRatio = frameBufferSize.x / frameBufferSize.y;

	Math::Vec2ui finalRes = static_cast<Math::Vec2ui>(frameBufferSize * renderScale);

	//Make sure the resolution is an integer scale of the framebuffer size.
	//This is done to avoid scaling artifacts (like blurriness).
	while((NumericCast<float>(finalRes.x) / NumericCast<float>(finalRes.y)) != aspectRatio)
	{
		if((NumericCast<float>(finalRes.x) / NumericCast<float>(finalRes.y)) <= aspectRatio)
			++finalRes.x;
		else
			++finalRes.y;
	}

	return finalRes;
}
#else
[[nodiscard]] TRAP::Math::Vec2ui TRAP::Graphics::RendererAPI::GetInternalRenderResolution()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const float renderScale = s_perViewportData->RenderScale;

	if(renderScale == 1.0f)
		return {s_perViewportData->NewWidth, s_perViewportData->NewHeight};

	const Math::Vec2 frameBufferSize
	{
		NumericCast<float>(s_perViewportData->NewWidth),
		NumericCast<float>(s_perViewportData->NewHeight),
	};
	const float aspectRatio = frameBufferSize.x / frameBufferSize.y;

	Math::Vec2ui finalRes = static_cast<Math::Vec2ui>(frameBufferSize * renderScale);

	//Make sure the resolution is an integer scale of the framebuffer size.
	//This is done to avoid scaling artifacts (like blurriness).
	while((NumericCast<float>(finalRes.x) / NumericCast<float>(finalRes.y)) != aspectRatio)
	{
		if((NumericCast<float>(finalRes.x) / NumericCast<float>(finalRes.y)) <= aspectRatio)
			++finalRes.x;
		else
			++finalRes.y;
	}

	return finalRes;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RendererAPI::StartRenderPass(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(window == nullptr)
		window = TRAP::Application::GetWindow();

	const auto* const viewportData = s_perViewportDataMap.at(window).get();

	TRAP::Ref<Graphics::RenderTarget> renderTarget = nullptr;

	//Get correct RenderTarget
	if((viewportData->RenderScale != 1.0f || s_currentAntiAliasing == RendererAPI::AntiAliasing::MSAA) && viewportData->State == PerWindowState::PreUpdate)
		renderTarget = viewportData->InternalRenderTargets[viewportData->CurrentSwapChainImageIndex];
	else
		renderTarget = viewportData->SwapChain->GetRenderTargets()[viewportData->CurrentSwapChainImageIndex];

	GetRenderer()->BindRenderTarget(renderTarget, nullptr, nullptr,
									nullptr, nullptr, std::numeric_limits<uint32_t>::max(),
									std::numeric_limits<uint32_t>::max(), window);
}
#else
void TRAP::Graphics::RendererAPI::StartRenderPass()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP::Ref<Graphics::RenderTarget> renderTarget = nullptr;

	//Get correct RenderTarget
	if((s_perViewportData->RenderScale != 1.0f || s_currentAntiAliasing == RendererAPI::AntiAliasing::MSAA) && s_perViewportData->State == PerWindowState::PreUpdate)
		renderTarget = s_perViewportData->InternalRenderTargets[s_perViewportData->ImageIndex];
	else
		renderTarget = s_perViewportData->RenderTargets[s_perViewportData->ImageIndex];

	GetRenderer()->BindRenderTarget(renderTarget, nullptr, nullptr,
	                                nullptr, nullptr, std::numeric_limits<uint32_t>::max(),
									std::numeric_limits<uint32_t>::max());
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RendererAPI::StopRenderPass(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(window == nullptr)
		window = TRAP::Application::GetWindow();

	GetRenderer()->BindRenderTarget(nullptr, nullptr, nullptr, nullptr, nullptr,
	                                std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max(),
									window);
}
#else
void TRAP::Graphics::RendererAPI::StopRenderPass()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	GetRenderer()->BindRenderTarget(nullptr, nullptr, nullptr, nullptr, nullptr,
	                                std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max());
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Transition(const Ref<TRAP::Graphics::Texture>& texture,
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

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RendererAPI::ResizeSwapChain(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (window == nullptr)
		window = TRAP::Application::GetWindow();

	s_perViewportDataMap.at(window)->ResizeSwapChain = true;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] float TRAP::Graphics::RendererAPI::GetGPUGraphicsFrameTime(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(window == nullptr)
		window = TRAP::Application::GetWindow();

	return s_perViewportDataMap.at(window)->GraphicsFrameTime;
}
#else
[[nodiscard]] float TRAP::Graphics::RendererAPI::GetGPUGraphicsFrameTime()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_perViewportData->GraphicsFrameTime;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] float TRAP::Graphics::RendererAPI::GetGPUComputeFrameTime(const Window* window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(window == nullptr)
		window = TRAP::Application::GetWindow();

	return s_perViewportDataMap.at(window)->ComputeFrameTime;
}
#else
[[nodiscard]] float TRAP::Graphics::RendererAPI::GetGPUComputeFrameTime()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_perViewportData->ComputeFrameTime;
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::RendererAPI::IsVulkanCapable()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!s_isVulkanCapableFirstTest)
		return s_isVulkanCapable;

	TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
	TP_INFO(Log::RendererVulkanPrefix, "Running Vulkan capability tester");

	s_isVulkanCapableFirstTest = false;

#ifndef TRAP_HEADLESS_MODE
	if (!INTERNAL::WindowingAPI::Init())
		Utils::DisplayError(Utils::ErrorCode::WindowingAPIFailedInitialization);
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	if (INTERNAL::WindowingAPI::VulkanSupported())
#else
	static const VkResult initRes = VkInitialize();
	if(initRes == VK_SUCCESS)
#endif /*TRAP_HEADLESS_MODE*/
	{
		if(VkGetInstanceVersion() < VK_API_VERSION_1_1)
		{
			TP_WARN(Log::RendererVulkanPrefix, "Failed instance version test!");
			TP_WARN(Log::RendererVulkanPrefix, "Failed Vulkan capability tester!");
			TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
			s_isVulkanCapable = false;
			return s_isVulkanCapable;
		}

		std::vector<std::string> instanceExtensions{};
#ifndef TRAP_HEADLESS_MODE
		//Required: Instance Extensions
		//Surface extensions are optional in Headless mode.
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

		GPUSettings.SurfaceSupported = true;
		instanceExtensions.push_back(std::get<0>(reqExt));
		instanceExtensions.push_back(std::get<1>(reqExt));
#endif /*TRAP_HEADLESS_MODE*/

		if(!API::VulkanInstance::IsExtensionSupported(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
		{
			TP_WARN(Log::RendererVulkanPrefix, "Failed required instance extension test");
			TP_WARN(Log::RendererVulkanPrefix, "Failed Vulkan capability tester!");
			TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
			s_isVulkanCapable = false;
			return s_isVulkanCapable;
		}

		instanceExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

		//Create Instance
		VkInstance instance = VK_NULL_HANDLE;
		std::vector<const char*> extensions(instanceExtensions.size());
		for (uint32_t i = 0; i < instanceExtensions.size(); i++)
			extensions[i] = instanceExtensions[i].c_str();
		const VkApplicationInfo appInfo = API::VulkanInits::ApplicationInfo("Vulkan Capability Tester");
		const VkInstanceCreateInfo instanceCreateInfo = API::VulkanInits::InstanceCreateInfo(appInfo, {}, extensions);
		const VkResult res = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
		if (res == VK_SUCCESS && (instance != nullptr))
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

TRAP::Graphics::RendererAPI::PerViewportData::~PerViewportData()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifndef TRAP_HEADLESS_MODE
	SwapChain.reset();
	ImageAcquiredSemaphore.reset();
#else
	RenderTargets = {};
#endif

	RenderCompleteSemaphores = {};
	RenderCompleteFences = {};

	for(uint32_t i = 0u; i < ImageCount; ++i)
		GraphicCommandPools[i]->FreeCommandBuffer(GraphicCommandBuffers[i]);
	GraphicCommandBuffers = {};
	GraphicCommandPools = {};

	GraphicsTimestampQueryPools = {};
	GraphicsTimestampReadbackBuffers = {};

	ComputeCompleteSemaphores = {};
	ComputeCompleteFences = {};

	for(uint32_t i = 0u; i < ImageCount; ++i)
		ComputeCommandPools[i]->FreeCommandBuffer(ComputeCommandBuffers[i]);
	ComputeCommandBuffers = {};
	ComputeCommandPools = {};

	ComputeTimestampQueryPools = {};
	ComputeTimestampReadbackBuffers = {};
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] uint32_t TRAP::Graphics::RendererAPI::GetCurrentImageIndex(const TRAP::Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "RendererAPI::GetCurrentImageIndex(): Window is nullptr!");

	return s_perViewportDataMap.at(window)->ImageIndex;
}
#else
[[nodiscard]] uint32_t TRAP::Graphics::RendererAPI::GetCurrentImageIndex()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_perViewportData->ImageIndex;
}
#endif /*TRAP_HEADLESS_MODE*/