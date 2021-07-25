#include "TRAPPCH.h"
#include "VulkanSurface.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"


TRAP::Graphics::API::VulkanSurface::VulkanSurface(TRAP::Ref<VulkanInstance> instance,
												  const TRAP::Ref<VulkanDevice>& device,
                                                  TRAP::Window* window)
	: m_surface(VK_NULL_HANDLE), m_surfaceCapabilities(), m_instance(std::move(instance))
{
	TRAP_ASSERT(m_instance, "instance is nullptr");
	TRAP_ASSERT(device, "device is nullptr");
	TRAP_ASSERT(window, "window is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSurfacePrefix, "Creating Surface");
#endif

	VkCall(TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(m_instance->GetVkInstance(),
	                                                         static_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow()),
															 nullptr, m_surface));

	VkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), m_surface,
	                                                 &m_surfaceCapabilities));

	uint32_t surfaceFormatCount = 0;
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), m_surface,
	                                            &surfaceFormatCount, nullptr));
	m_surfaceFormats.resize(surfaceFormatCount);
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), m_surface,
	                                            &surfaceFormatCount, m_surfaceFormats.data()));

	uint32_t surfacePresentCount = 0;
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), m_surface,
	                                                 &surfacePresentCount, nullptr));
	m_surfacePresentModes.resize(surfacePresentCount);
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), m_surface,
	                                                 &surfacePresentCount, m_surfacePresentModes.data()));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSurface::~VulkanSurface()
{
	TRAP_ASSERT(m_surface);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSurfacePrefix, "Destroying Surface");
#endif

	vkDestroySurfaceKHR(m_instance->GetVkInstance(), m_surface, nullptr);
	m_surface = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSurfaceKHR& TRAP::Graphics::API::VulkanSurface::GetVkSurface()
{
	return m_surface;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSurfaceCapabilitiesKHR TRAP::Graphics::API::VulkanSurface::GetVkSurfaceCapabilities() const
{
	return m_surfaceCapabilities;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkSurfaceFormatKHR>& TRAP::Graphics::API::VulkanSurface::GetVkSurfaceFormats() const
{
	return m_surfaceFormats;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkPresentModeKHR>& TRAP::Graphics::API::VulkanSurface::GetVkSurfacePresentModes() const
{
	return m_surfacePresentModes;
}