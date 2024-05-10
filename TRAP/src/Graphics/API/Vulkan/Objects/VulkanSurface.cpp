#include "TRAPPCH.h"
#include "VulkanSurface.h"

#ifndef TRAP_HEADLESS_MODE

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

namespace
{
	[[nodiscard]] std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(const TRAP::Graphics::API::VulkanDevice& device, VkSurfaceKHR surface)
	{
		std::vector<VkSurfaceFormatKHR> formats{};

		u32 surfaceFormatCount = 0;
		VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice().GetVkPhysicalDevice(), surface,
													&surfaceFormatCount, nullptr));
		formats.resize(surfaceFormatCount);
		VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice().GetVkPhysicalDevice(), surface,
													&surfaceFormatCount, formats.data()));

		return formats;
	}

	[[nodiscard]] std::vector<VkPresentModeKHR> GetSurfacePresentMode(const TRAP::Graphics::API::VulkanDevice& device, VkSurfaceKHR surface)
	{
		std::vector<VkPresentModeKHR> presentModes{};

		u32 surfacePresentCount = 0;
		VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice().GetVkPhysicalDevice(), surface,
														&surfacePresentCount, nullptr));
		presentModes.resize(surfacePresentCount);
		VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice().GetVkPhysicalDevice(), surface,
														&surfacePresentCount, presentModes.data()));

		return presentModes;
	}
}

TRAP::Graphics::API::VulkanSurface::VulkanSurface(TRAP::Ref<VulkanInstance> instance,
												  const VulkanDevice& device,
                                                  const TRAP::Window& window)
	: m_instance(std::move(instance))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_instance, "VulkanSurface(): Vulkan Instance is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSurfacePrefix, "Creating Surface");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	VkCall(TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(m_instance->GetVkInstance(),
	                                                         *static_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>(window.GetInternalWindow()),
															 nullptr, m_surface));
	TRAP_ASSERT(m_surface, "VulkanSurface(): Vulkan Surface is nullptr");

	VkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysicalDevice().GetVkPhysicalDevice(), m_surface,
	                                                 &m_surfaceCapabilities));

	m_surfaceFormats = GetSurfaceFormats(device, m_surface);
	m_surfacePresentModes = GetSurfacePresentMode(device, m_surface);

#ifdef ENABLE_GRAPHICS_DEBUG
	if(!window.GetTitle().empty())
		TRAP::Graphics::API::VkSetObjectName(device.GetVkDevice(), std::bit_cast<u64>(m_surface), VK_OBJECT_TYPE_SURFACE_KHR, window.GetTitle());
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSurface::~VulkanSurface()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSurfacePrefix, "Destroying Surface");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroySurfaceKHR(m_instance->GetVkInstance(), m_surface, nullptr);
}

#endif /*TRAP_HEADLESS_MODE*/
