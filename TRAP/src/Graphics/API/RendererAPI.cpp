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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::RendererAPI> TRAP::Graphics::RendererAPI::s_Renderer = nullptr;
TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::s_RenderAPI = TRAP::Graphics::RenderAPI::NONE;
TRAP::Scope<TRAP::Graphics::API::ResourceLoader> TRAP::Graphics::RendererAPI::s_ResourceLoader = nullptr;
std::unordered_map<TRAP::Window*, TRAP::Scope<TRAP::Graphics::RendererAPI::PerWindowData>> TRAP::Graphics::RendererAPI::s_perWindowDataMap = {};
std::mutex TRAP::Graphics::RendererAPI::s_perWindowDataMutex{};
bool TRAP::Graphics::RendererAPI::s_isVulkanCapable = true;
bool TRAP::Graphics::RendererAPI::s_isVulkanCapableFirstTest = true;
TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::RendererAPI::s_descriptorPool = nullptr;
TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::s_graphicQueue = nullptr;
TRAP::Ref<TRAP::Graphics::Queue> TRAP::Graphics::RendererAPI::s_computeQueue = nullptr;
std::array<TRAP::Ref<TRAP::Graphics::CommandPool>, TRAP::Graphics::RendererAPI::ImageCount> TRAP::Graphics::RendererAPI::s_computeCommandPools{};
std::array<TRAP::Graphics::CommandBuffer*, TRAP::Graphics::RendererAPI::ImageCount> TRAP::Graphics::RendererAPI::s_computeCommandBuffers{};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Init(const std::string& gameName)
{
	if(s_Renderer)
		return;

	switch (s_RenderAPI)
	{
	case RenderAPI::Vulkan:
		TP_INFO(Log::RendererVulkanPrefix, "Initializing Renderer");
		s_Renderer = MakeScope<API::VulkanRenderer>();
		break;

	default:
		TP_CRITICAL(Log::RendererPrefix, "Unsupported Device!");
		Utils::Dialogs::ShowMsgBox("Unsupported Device",
			"Device is unsupported!\n No RenderAPI selected!",
			Utils::Dialogs::Style::Error, Utils::Dialogs::Buttons::Quit);
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
	{
		std::lock_guard<std::mutex> lock(s_perWindowDataMutex);
		s_perWindowDataMap.clear();
	}

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
	return s_Renderer;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::API::ResourceLoader>& TRAP::Graphics::RendererAPI::GetResourceLoader()
{
	return s_ResourceLoader;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::AutoSelectRenderAPI()
{
	TP_INFO(Log::RendererPrefix, "Auto selecting RenderAPI");

	//Check if Vulkan capable
	if (s_Renderer->IsVulkanCapable())
	{
		s_RenderAPI = RenderAPI::Vulkan;
		return;
	}
	TP_DEBUG(Log::RendererVulkanPrefix, "Device isn't Vulkan 1.2 capable!");

	s_RenderAPI = RenderAPI::NONE;
	TRAP::Utils::Dialogs::ShowMsgBox("No compatible RenderAPI found",
		"TRAP was unable to detect a compatible RenderAPI!\nPlease check your GPU driver!",
		Utils::Dialogs::Style::Error,
		Utils::Dialogs::Buttons::Quit);
	TRAP::Application::Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::SwitchRenderAPI(const RenderAPI api)
{
	if (api != s_RenderAPI)
	{
		if (api == RenderAPI::Vulkan)
		{
			if (s_Renderer->IsVulkanCapable())
			{
				TP_WARN(Log::RendererPrefix, "Switching RenderAPI to Vulkan");
				s_RenderAPI = RenderAPI::Vulkan;

				return;
			}

			TP_ERROR(Log::RendererVulkanPrefix, "This device doesn't support Vulkan 1.2!");
			
			TRAP::Utils::Dialogs::ShowMsgBox("No compatible RenderAPI found",
				"TRAP was unable to detect a compatible RenderAPI!\nPlease check your GPU driver!",
				Utils::Dialogs::Style::Error,
				Utils::Dialogs::Buttons::Quit);
			TRAP::Application::Shutdown();
		}
		else
		{
			TRAP::Utils::Dialogs::ShowMsgBox("No compatible RenderAPI found",
				"TRAP was unable to detect a compatible RenderAPI!\nPlease check your GPU driver!",
				Utils::Dialogs::Style::Error,
				Utils::Dialogs::Buttons::Quit);
			TRAP::Application::Shutdown();
		}
	}
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

const TRAP::Scope<TRAP::Graphics::RendererAPI::PerWindowData>& TRAP::Graphics::RendererAPI::GetPerWindowData(Window* window)
{
	return s_perWindowDataMap[window];
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

	return std::get<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>(s_perWindowDataMap[window]->GraphicsPipelineDesc.Pipeline).RootSignature;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RendererAPI::IsVulkanCapable()
{
	if (s_isVulkanCapableFirstTest)
	{
		TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
		TP_INFO(Log::RendererVulkanPrefix, "Running Vulkan Capability Tester");
		
		s_isVulkanCapableFirstTest = false;
		
		if (INTERNAL::WindowingAPI::VulkanSupported())
		{
			if(VkGetInstanceVersion() < VK_API_VERSION_1_2)
			{
				TP_CRITICAL(Log::RendererVulkanPrefix, "Failed Instance version Test!");
				TP_CRITICAL(Log::RendererVulkanPrefix, "Failed Vulkan Capability Tester!");
				TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
				s_isVulkanCapable = false;
				return s_isVulkanCapable;
			}
			
			//Instance Extensions
			std::vector<std::string> instanceExtensions{};
			const auto reqExt = INTERNAL::WindowingAPI::GetRequiredInstanceExtensions();
			if (!API::VulkanInstance::IsExtensionSupported(reqExt[0]) || !API::VulkanInstance::IsExtensionSupported(reqExt[1]))
			{
				TP_CRITICAL(Log::RendererVulkanPrefix, "Failed Surface Extension Test");
				TP_CRITICAL(Log::RendererVulkanPrefix, "Failed Vulkan Capability Tester!");
				TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
				s_isVulkanCapable = false;
				return s_isVulkanCapable;
			}
			instanceExtensions.push_back(reqExt[0]);
			instanceExtensions.push_back(reqExt[1]);

			//Create Instance
			VkInstance instance;
			std::vector<const char*> extensions(instanceExtensions.size());
			for (uint32_t i = 0; i < instanceExtensions.size(); i++)
				extensions[i] = instanceExtensions[i].c_str();
			const VkApplicationInfo appInfo = API::VulkanInits::ApplicationInfo("Vulkan Capability Tester");
			VkInstanceCreateInfo instanceCreateInfo = API::VulkanInits::InstanceCreateInfo(appInfo, {}, extensions);
			VkCall(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));			
			if (instance)
			{
				VkLoadInstance(instance);

				//Physical Device
				const auto& physicalDevices = API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(instance);
				if(physicalDevices.empty())
				{
					s_isVulkanCapable = false;
					TP_CRITICAL(Log::RendererVulkanPrefix, "Failed to find a suitable GPU meeting all requirements!");
				}
			}
			else
			{
				s_isVulkanCapable = false;
				TP_CRITICAL(Log::RendererVulkanPrefix, "Failed to create Vulkan Instance!");
			}
		}
		else
			s_isVulkanCapable = false;

		if (s_isVulkanCapable)
		{
			TP_INFO(Log::RendererVulkanPrefix, "Passed Vulkan Capability Tester!");
			TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
		}
		else
		{
			TP_CRITICAL(Log::RendererVulkanPrefix, "Failed Vulkan Capability Tester!");
			TP_INFO(Log::RendererVulkanPrefix, "--------------------------------");
		}
	}
	
	return s_isVulkanCapable;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::PerWindowData::~PerWindowData()
{
	SwapChain.reset();
	ImageAcquiredSemaphore.reset();

	for(uint32_t i = ImageCount; i > 0; i--)
	{
		RenderCompleteSemaphores[i - 1].reset();
		RenderCompleteFences[i - 1].reset();

		GraphicCommandPools[i - 1]->FreeCommandBuffer(GraphicCommandBuffers[i - 1]);
		GraphicCommandBuffers[i - 1] = nullptr;
		GraphicCommandPools[i - 1].reset();
	}
}