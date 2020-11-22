#include "TRAPPCH.h"
#include "RendererAPI.h"

#include "Application.h"
#include "Vulkan/VulkanRenderer.h"
#include "Utils/Dialogs/MsgBox.h"
#include "Vulkan/VulkanCommon.h"
#include "Vulkan/Objects/VulkanInits.h"
#include "Vulkan/Objects/VulkanInstance.h"
#include "Vulkan/Objects/VulkanPhysicalDevice.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::RendererAPI> TRAP::Graphics::RendererAPI::s_Renderer = nullptr;
TRAP::Graphics::RenderAPI TRAP::Graphics::RendererAPI::s_RenderAPI = TRAP::Graphics::RenderAPI::NONE;
bool TRAP::Graphics::RendererAPI::s_isVulkanCapable = true;
bool TRAP::Graphics::RendererAPI::s_isVulkanCapableFirstTest = true;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Init()
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

	s_Renderer->InitInternal();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Shutdown()
{
	s_Renderer.reset();
	s_Renderer = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::RendererAPI>& TRAP::Graphics::RendererAPI::GetRenderer()
{
	return s_Renderer;
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
	TP_DEBUG(Log::RendererVulkanPrefix, "Device isn't Vulkan capable!");

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

			TP_ERROR(Log::RendererVulkanPrefix, "This device doesn't support Vulkan!");
			
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

void TRAP::Graphics::RendererAPI::SetVSync(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RendererAPI::GetVSync()
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RendererAPI::Use(Window* window)
{
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
			//Instance Extensions
			std::vector<std::string> instanceExtensions{};
			const auto reqExt = INTERNAL::WindowingAPI::GetRequiredInstanceExtensions();
			if (!API::VulkanInstance::IsExtensionSupported(reqExt[0]) || !API::VulkanInstance::IsExtensionSupported(reqExt[1]))
			{
				s_isVulkanCapable = false;
				return s_isVulkanCapable;
			}
			instanceExtensions.push_back(reqExt[0]);
			instanceExtensions.push_back(reqExt[1]);
			if (API::VulkanInstance::GetInstanceVersion() < VK_MAKE_VERSION(1, 1, 0) && !API::VulkanInstance::IsExtensionSupported(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME))
			{
				s_isVulkanCapable = false;
				return s_isVulkanCapable;
			}
			instanceExtensions.emplace_back(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
			if (API::VulkanInstance::GetInstanceVersion() < VK_MAKE_VERSION(1, 1, 0) && !API::VulkanInstance::IsExtensionSupported(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
			{
				s_isVulkanCapable = false;
				return s_isVulkanCapable;
			}
			instanceExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

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