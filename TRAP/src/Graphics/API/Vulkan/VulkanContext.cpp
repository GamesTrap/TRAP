#include "TRAPPCH.h"
#include "VulkanContext.h"

#include "Window/Window.h"
#include "Internals/Objects/VulkanInstance.h"
#include "Internals/Objects/VulkanPhysicalDevice.h"
#include "VulkanCommon.h"
#include "VulkanRenderer.h"
#include "Internals/Objects/VulkanSwapchain.h"
#include "Internals/Objects/VulkanCommandBuffer.h"

//-------------------------------------------------------------------------------------------------------------------//

PFN_vkEnumerateInstanceVersion TRAP::Graphics::API::VulkanContext::s_vkEnumerateInstanceVersion = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::InternalWindow* TRAP::Graphics::API::VulkanContext::s_currentWindow = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VulkanContext::s_appName;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanContext::VulkanContext(Window* window)
{
	TP_PROFILE_FUNCTION();

	s_currentWindow = static_cast<INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow());
	s_appName = window->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanContext::~VulkanContext()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG(Log::ContextVulkanPrefix, "Destroying Context");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::SetVSyncIntervalInternal(const uint32_t interval)
{
	TP_PROFILE_FUNCTION();

	if(s_currentWindow)
		VulkanRenderer::SetVSyncIntervalInternal(interval);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::UseInternal(Window* window)
{
	TP_PROFILE_FUNCTION();

	if(window->GetInternalWindow() != s_currentWindow)
	{
		//Switch swapchains to the new Window
		s_currentWindow = static_cast<INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow());

		for(const Scope<Vulkan::Swapchain>& swapchain : VulkanRenderer::GetAllSwapchains())
		{
			if (swapchain->GetBoundWindow() == window->GetInternalWindow())
			{
				VulkanRenderer::SetCurrentSwapchain(swapchain.get());
				return;
			}
		}

		//No swapchain was found :C
		Vulkan::Swapchain* swapchain = VulkanRenderer::CreateWindowSwapchain(s_currentWindow);
		VulkanRenderer::SetCurrentSwapchain(swapchain);

		//Start Recording
		VulkanRenderer::GetCurrentSwapchain().StartGraphicCommandBuffersAndRenderPass();

		//Set Viewport and scissor
		VkViewport viewport{ 0.0f, 0.0f, static_cast<float>(s_currentWindow->Width), static_cast<float>(s_currentWindow->Height), 0.0f, 1.0f };
		VkRect2D scissor{ {0, 0}, {static_cast<uint32_t>(s_currentWindow->Width), static_cast<uint32_t>(s_currentWindow->Height)} };
		VulkanRenderer::GetCurrentSwapchain().GetGraphicsCommandBuffer().SetViewport(viewport);
		VulkanRenderer::GetCurrentSwapchain().GetGraphicsCommandBuffer().SetScissor(scissor);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanContext::IsVulkanCapable()
{
	TP_PROFILE_FUNCTION();
	
#define TRAP_EXPERIMENTAL_VULKAN
	
#ifndef TRAP_EXPERIMENTAL_VULKAN
	return false;
#endif

	TP_INFO(Log::ContextVulkanPrefix, "-------------------------");
	TP_INFO(Log::ContextVulkanPrefix, "Running Vulkan 1.2 Tester");
	if (INTERNAL::WindowingAPI::VulkanSupported())
	{
		//Validate that Vulkan Instance version is Vulkan 1.2 or newer
		s_vkEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));
		uint32_t instanceVersion = 0;
		VkCall(s_vkEnumerateInstanceVersion(&instanceVersion));
		if (instanceVersion < VK_API_VERSION_1_2)
		{
			TP_CRITICAL(Log::ContextVulkanPrefix, "Instance Version Does Not Support Vulkan 1.2 Instances!");
			return false;
		}
		
		//Get Required and Available Extensions
		std::array<std::string, 2> requiredExtensions = INTERNAL::WindowingAPI::GetRequiredInstanceExtensions();
		for(std::string_view str : requiredExtensions)
		{
			if (!Vulkan::Instance::IsExtensionSupported(str.data()))
			{
				TP_CRITICAL(Log::ContextVulkanPrefix, "Required Instance Extension: \"", str, "\" Is NOT Supported!");
				return false;
			}
		}

		//Test if Vulkan is able to create an instance
		std::vector<std::string> layers{};
		std::vector<std::string> extensions{ requiredExtensions[0], requiredExtensions[1]};
		Scope<Vulkan::Instance> instance = MakeScope<Vulkan::Instance>("TRAP Vulkan Capability Tester", TRAP_VERSION, layers, extensions);
		if (!instance->GetInstance())
		{
			TP_CRITICAL(Log::ContextVulkanPrefix, "Failed To Create Vulkan 1.2 Instance!");
			return false;
		}

		//Create Vulkan 1.2 Test Window
		INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Visible, false);
		INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Focused, false);
		Scope<INTERNAL::WindowingAPI::InternalWindow> vulkanTestWindow = INTERNAL::WindowingAPI::CreateWindow(400, 400, "TRAP Vulkan Capability Tester", nullptr);
		INTERNAL::WindowingAPI::DefaultWindowHints();
		if (!vulkanTestWindow)
		{
			TP_CRITICAL(Log::ContextVulkanPrefix, "Failed to create Vulkan 1.2 Test Window!");
			return false;
		}

		//Get All Available Graphic Physical Devices and check if it is empty
		const auto& physicalDevices = Vulkan::PhysicalDevice::GetAllAvailableGraphicPhysicalDevices(instance, vulkanTestWindow.get());
		if (physicalDevices.empty())
		{
			INTERNAL::WindowingAPI::DestroyWindow(std::move(vulkanTestWindow));

			TP_CRITICAL(Log::ContextVulkanPrefix, "Failed To Find a Suitable GPU Meeting All Requirements!");
			return false;
		}

		INTERNAL::WindowingAPI::DestroyWindow(std::move(vulkanTestWindow));

		TP_INFO(Log::ContextVulkanPrefix, "Passed Vulkan 1.2 Tester");
		TP_INFO(Log::ContextVulkanPrefix, "------------------------");
		return true;
	}

	TP_CRITICAL(Log::ContextVulkanPrefix, "Failed Vulkan 1.2 Support Checks!");
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::InternalWindow* TRAP::Graphics::API::VulkanContext::GetCurrentWindow()
{
	return s_currentWindow;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::API::VulkanContext::GetAppName()
{
	return s_appName;
}