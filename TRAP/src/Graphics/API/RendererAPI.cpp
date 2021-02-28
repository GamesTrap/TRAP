#include "TRAPPCH.h"
#include "RendererAPI.h"

#include "Application.h"
#include "Vulkan/VulkanRenderer.h"
#include "Utils/Dialogs/MsgBox.h"
#include "Vulkan/VulkanCommon.h"
#include "Vulkan/Objects/VulkanInits.h"
#include "Vulkan/Objects/VulkanInstance.h"
#include "Vulkan/Objects/VulkanPhysicalDevice.h"
#include "ResourceLoader.h"
#include "Objects/CommandPool.h"
#include "Objects/Queue.h"
#include "Vulkan/Objects/VulkanDevice.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::RendererAPI> TRAP::Graphics::RendererAPI::s_Renderer = nullptr;
TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::s_RenderAPI = TRAP::Graphics::RenderAPI::NONE;
TRAP::Scope<TRAP::Graphics::API::ResourceLoader> TRAP::Graphics::RendererAPI::s_ResourceLoader = nullptr;
std::unordered_map<TRAP::Window*, TRAP::Scope<TRAP::Graphics::RendererAPI::PerWindowData>> TRAP::Graphics::RendererAPI::s_perWindowDataMap = {};
std::mutex TRAP::Graphics::RendererAPI::s_perWindowDataMutex{};
bool TRAP::Graphics::RendererAPI::s_isVulkanCapable = true;
bool TRAP::Graphics::RendererAPI::s_isVulkanCapableFirstTest = true;
TRAP::Ref<TRAP::Graphics::DescriptorPool> TRAP::Graphics::RendererAPI::s_descriptorPool = nullptr;
TRAP::Graphics::RendererAPI::RootSignatureDesc TRAP::Graphics::RendererAPI::s_graphicRootSignatureDesc{};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Init(const std::string& gameName)
{	
	switch (s_RenderAPI)
	{
	case RenderAPI::Vulkan:
		TP_INFO(Log::RendererVulkanPrefix, "Initializing Renderer");
		s_Renderer = MakeScope<API::VulkanRenderer>();
		break;

	default:
		TP_CRITICAL(Log::RendererPrefix, "Unsupported Device!");
		Utils::Dialogs::MsgBox::Show("Device is unsupported!\n No RenderAPI selected!", "Unsupported Device", Utils::Dialogs::MsgBox::Style::Error, Utils::Dialogs::MsgBox::Buttons::Quit);
		exit(-1);
	}

	s_Renderer->InitInternal(gameName);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Shutdown()
{
	{
		std::lock_guard<std::mutex> lock(s_perWindowDataMutex);
		s_perWindowDataMap.clear();
	}
	
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
	TRAP::Utils::Dialogs::MsgBox::Show("TRAP was unable to detect a compatible RenderAPI!\nPlease check your GPU driver!",
		"No compatible RenderAPI found",
		Utils::Dialogs::MsgBox::Style::Error,
		Utils::Dialogs::MsgBox::Buttons::Quit);
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
			
			TRAP::Utils::Dialogs::MsgBox::Show("TRAP was unable to detect a compatible RenderAPI!\nPlease check your GPU driver!",
				"No compatible RenderAPI found",
				Utils::Dialogs::MsgBox::Style::Error,
				Utils::Dialogs::MsgBox::Buttons::Quit);
			TRAP::Application::Shutdown();
		}
		else
		{
			TRAP::Utils::Dialogs::MsgBox::Show("TRAP was unable to detect a compatible RenderAPI!\nPlease check your GPU driver!",
				"No compatible RenderAPI found",
				Utils::Dialogs::MsgBox::Style::Error,
				Utils::Dialogs::MsgBox::Buttons::Quit);
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

void TRAP::Graphics::RendererAPI::RemoveShaderFromGraphicsRootSignature(Shader* shader)
{
	if (s_graphicRootSignatureDesc.Shaders.empty())
		return;

	if(std::find(s_graphicRootSignatureDesc.Shaders.begin(), s_graphicRootSignatureDesc.Shaders.end(), shader) != s_graphicRootSignatureDesc.Shaders.end())
		s_graphicRootSignatureDesc.Shaders.erase(std::remove(s_graphicRootSignatureDesc.Shaders.begin(), s_graphicRootSignatureDesc.Shaders.end(), shader));
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
	GraphicQueue->WaitQueueIdle();
	ComputeQueue->WaitQueueIdle();

	SwapChain.reset();
	ImageAcquiredSemaphore.reset();

	for(uint32_t i = ImageCount - 1; i > 0; i--)
	{
		RenderCompleteSemaphores[i].reset();
		RenderCompleteFences[i].reset();

		GraphicCommandPools[i]->FreeCommandBuffer(GraphicCommandBuffers[i]);
		GraphicCommandPools[i].reset();

		ComputeCommandPools[i]->FreeCommandBuffer(ComputeCommandBuffers[i]);
		ComputeCommandPools[i].reset();
	}

	GraphicQueue.reset();
	ComputeQueue.reset();
}