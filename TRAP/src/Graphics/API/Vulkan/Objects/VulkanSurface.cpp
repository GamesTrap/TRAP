#include "TRAPPCH.h"
#include "VulkanSurface.h"

#ifndef TRAP_HEADLESS_MODE

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"


TRAP::Graphics::API::VulkanSurface::VulkanSurface(TRAP::Ref<VulkanInstance> instance,
												  const TRAP::Ref<VulkanDevice>& device,
                                                  const TRAP::Window* const window)
	: m_instance(std::move(instance))
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_instance, "VulkanSurface(): Vulkan Instance is nullptr");
	TRAP_ASSERT(device, "VulkanSurface(): Vulkan Device is nullptr");
	TRAP_ASSERT(window, "VulkanSurface(): Window is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSurfacePrefix, "Creating Surface");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	VkCall(TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(m_instance->GetVkInstance(),
	                                                         *static_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow()),
															 nullptr, m_surface));
	TRAP_ASSERT(m_surface, "VulkanSurface(): Vulkan Surface is nullptr");

	VkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), m_surface,
	                                                 &m_surfaceCapabilities));

	u32 surfaceFormatCount = 0;
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), m_surface,
	                                            &surfaceFormatCount, nullptr));
	m_surfaceFormats.resize(surfaceFormatCount);
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), m_surface,
	                                            &surfaceFormatCount, m_surfaceFormats.data()));

	u32 surfacePresentCount = 0;
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), m_surface,
	                                                 &surfacePresentCount, nullptr));
	m_surfacePresentModes.resize(surfacePresentCount);
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), m_surface,
	                                                 &surfacePresentCount, m_surfacePresentModes.data()));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSurface::~VulkanSurface()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSurfacePrefix, "Destroying Surface");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroySurfaceKHR(m_instance->GetVkInstance(), m_surface, nullptr);
}

#endif /*TRAP_HEADLESS_MODE*/