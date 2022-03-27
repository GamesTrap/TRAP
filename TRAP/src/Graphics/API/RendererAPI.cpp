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
#include "Objects/Queue.h"
#include "Objects/Sampler.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::RendererAPI> TRAP::Graphics::RendererAPI::s_Renderer = nullptr;
TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::s_RenderAPI = TRAP::Graphics::RenderAPI::NONE;
TRAP::Scope<TRAP::Graphics::API::ResourceLoader> TRAP::Graphics::RendererAPI::s_ResourceLoader = nullptr;
std::unordered_map<TRAP::Window*,
                   TRAP::Scope<TRAP::Graphics::RendererAPI::PerWindowData>> TRAP::Graphics::RendererAPI::s_perWindowDataMap = {};
bool TRAP::Graphics::RendererAPI::s_isVulkanCapable = true;
bool TRAP::Graphics::RendererAPI::s_isVulkanCapableFirstTest = true;
TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::RendererAPI::s_descriptorPool = nullptr;
TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::s_graphicQueue = nullptr;
TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::s_computeQueue = nullptr;
std::array<TRAP::Ref<TRAP::Graphics::CommandPool>,
           TRAP::Graphics::RendererAPI::ImageCount> TRAP::Graphics::RendererAPI::s_computeCommandPools{};
std::array<TRAP::Graphics::CommandBuffer*,
           TRAP::Graphics::RendererAPI::ImageCount> TRAP::Graphics::RendererAPI::s_computeCommandBuffers{};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Init(const std::string_view gameName, const RenderAPI renderAPI)
{
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
		TP_CRITICAL(Log::RendererPrefix, "Unsupported device!");
		TRAP::Application::Shutdown();
		exit(-1);
	}

	s_Renderer->InitInternal(gameName);

	//Create Graphic Queue
	QueueDesc queueDesc{};
	queueDesc.Type = QueueType::Graphics;
	queueDesc.Flag = QueueFlag::InitMicroprofile;
	s_graphicQueue = Queue::Create(queueDesc);

	//Create Compute Queue
	queueDesc.Type = QueueType::Compute;
	s_computeQueue = Queue::Create(queueDesc);

	//For each buffered image
	for (uint32_t i = 0; i < RendererAPI::ImageCount; ++i)
	{
		//Create Compute Command Pool
		s_computeCommandPools[i] = CommandPool::Create({ s_computeQueue, false });
		//Allocate Compute Command Buffer
		s_computeCommandBuffers[i] = s_computeCommandPools[i]->AllocateCommandBuffer(false);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Shutdown()
{
	s_perWindowDataMap.clear();

	TRAP::Graphics::Sampler::ClearCache();

	for(uint32_t i = ImageCount; i > 0; i--)
	{
		s_computeCommandPools[i - 1]->FreeCommandBuffer(s_computeCommandBuffers[i - 1]);
		s_computeCommandBuffers[i - 1] = nullptr;
		s_computeCommandPools[i - 1].reset();
	}

	s_Renderer->s_graphicQueue->WaitQueueIdle();
	s_Renderer->s_computeQueue->WaitQueueIdle();
	s_Renderer->s_graphicQueue.reset();
	s_Renderer->s_computeQueue.reset();

	s_Renderer.reset();
	s_Renderer = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::RendererAPI>& TRAP::Graphics::RendererAPI::GetRenderer()
{
#ifdef TRAP_HEADLESS_MODE
	TRAP_ASSERT(s_RenderAPI != RenderAPI::NONE , "RendererAPI is not available because RenderAPI::NONE is set (or EnableGPU=False)!");
#endif

	return s_Renderer;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::API::ResourceLoader>& TRAP::Graphics::RendererAPI::GetResourceLoader()
{
	return s_ResourceLoader;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::AutoSelectRenderAPI()
{
	TP_INFO(Log::RendererPrefix, "Auto selecting RenderAPI");

	//Check if Vulkan capable
	if (s_Renderer->IsVulkanCapable())
		return RenderAPI::Vulkan;
	TP_WARN(Log::RendererVulkanPrefix, "Device isn't Vulkan 1.2 capable!");


#ifndef TRAP_HEADLESS_MODE
	s_RenderAPI = RenderAPI::NONE;
	TRAP::Utils::Dialogs::ShowMsgBox("Incompatible device (GPU)",
		                             "TRAP™ was unable to detect a compatible RenderAPI!\n"
									 "Does your system meet the minimum system requirements for running TRAP™?\n"
									 "Please check your GPU driver!\nError code: 0x000B", Utils::Dialogs::Style::Error,
		Utils::Dialogs::Buttons::Quit);
	TRAP::Application::Shutdown();
	exit(-1);
#else
	TP_WARN(Log::RendererVulkanPrefix, "Disabling RendererAPI, no compatible RenderAPI was found!");
	return RenderAPI::NONE;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RendererAPI::IsSupported(const RenderAPI api)
{
	if (api == RenderAPI::Vulkan)
		return s_Renderer->IsVulkanCapable();

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::GetRenderAPI()
{
	return s_RenderAPI;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::RendererAPI::GetDescriptorPool()
{
	return s_descriptorPool;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::GetGraphicsQueue()
{
	return s_graphicQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::GetComputeQueue()
{
	return s_computeQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::RendererAPI::PerWindowData>& TRAP::Graphics::RendererAPI::GetMainWindowData()
{
	return s_perWindowDataMap[TRAP::Application::GetWindow().get()];
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::RendererAPI::GetGraphicsRootSignature(Window* window)
{
	if (!window)
		window = TRAP::Application::GetWindow().get();

	return std::get<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>
	(
		s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline
	).RootSignature;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RendererAPI::IsVulkanCapable()
{
	if(!s_isVulkanCapableFirstTest)
		return s_isVulkanCapable;

	TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
	TP_INFO(Log::RendererVulkanPrefix, "Running Vulkan capability tester");

	s_isVulkanCapableFirstTest = false;

	if (INTERNAL::WindowingAPI::VulkanSupported())
	{
		if(VkGetInstanceVersion() < VK_API_VERSION_1_2)
		{
			TP_CRITICAL(Log::RendererVulkanPrefix, "Failed instance version test!");
			TP_CRITICAL(Log::RendererVulkanPrefix, "Failed Vulkan capability tester!");
			TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
			s_isVulkanCapable = false;
			return s_isVulkanCapable;
		}

		//Required: Instance Extensions
		//Optional in Headless mode.
		std::vector<std::string> instanceExtensions{};
		const auto reqExt = INTERNAL::WindowingAPI::GetRequiredInstanceExtensions();
		if (!API::VulkanInstance::IsExtensionSupported(reqExt[0]) ||
			!API::VulkanInstance::IsExtensionSupported(reqExt[1]))
		{
			GPUSettings.SurfaceSupported = false;
			TP_CRITICAL(Log::RendererVulkanPrefix, "Failed surface extension test");
#ifndef TRAP_HEADLESS_MODE
			TP_CRITICAL(Log::RendererVulkanPrefix, "Failed Vulkan capability tester!");
			TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
			s_isVulkanCapable = false;
			return s_isVulkanCapable;
#endif
		}
		else
		{
			GPUSettings.SurfaceSupported = true;
			instanceExtensions.push_back(reqExt[0]);
			instanceExtensions.push_back(reqExt[1]);
		}

		//Create Instance
		VkInstance instance = VK_NULL_HANDLE;
		std::vector<const char*> extensions(instanceExtensions.size());
		for (uint32_t i = 0; i < instanceExtensions.size(); i++)
			extensions[i] = instanceExtensions[i].c_str();
		const VkApplicationInfo appInfo = API::VulkanInits::ApplicationInfo("Vulkan Capability Tester");
		VkInstanceCreateInfo instanceCreateInfo = API::VulkanInits::InstanceCreateInfo(appInfo, {}, extensions);
		VkResult res = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
		if (res == VK_SUCCESS && instance)
		{
			VkLoadInstance(instance);

			//Physical Device
			const auto& physicalDevices = API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(instance);
			if(physicalDevices.empty())
			{
				s_isVulkanCapable = false;
				TP_CRITICAL(Log::RendererVulkanPrefix,
							"Failed to find a suitable GPU meeting all requirements!");
			}
		}
		else
		{
			s_isVulkanCapable = false;
			TP_CRITICAL(Log::RendererVulkanPrefix, "Failed to create Vulkan instance!");
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
		TP_CRITICAL(Log::RendererVulkanPrefix, "Failed Vulkan capability tester!");
		TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
	}

	return s_isVulkanCapable;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::PerWindowData::~PerWindowData()
{
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
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RendererAPI::SamplerDesc::operator==(const SamplerDesc& s) const
{
	//Deep equality
	return this->MinFilter == s.MinFilter && this->MagFilter == s.MagFilter && this->MipMapMode == s.MipMapMode &&
		   this->AddressU == s.AddressU && this->AddressV == s.AddressV && this->AddressW == s.AddressW &&
		   this->MipLodBias == s.MipLodBias && this->MaxAnisotropy == s.MaxAnisotropy &&
		   this->CompareFunc == s.CompareFunc && this->SamplerConversionDesc == s.SamplerConversionDesc;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RendererAPI::SamplerDesc::SamplerConversionDesc::operator==(const SamplerConversionDesc& s) const
{
	//Deep equality
	return this->Format == s.Format && this->Model == s.Model && this->Range == s.Range &&
	       this->ChromaOffsetX == s.ChromaOffsetX && this->ChromaOffsetY == s.ChromaOffsetY &&
		   this->ChromaFilter == s.ChromaFilter &&
		   this->ForceExplicitReconstruction == s.ForceExplicitReconstruction;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RendererAPI::GetCurrentImageIndex(TRAP::Window* window)
{
	return s_perWindowDataMap[window]->ImageIndex;
}