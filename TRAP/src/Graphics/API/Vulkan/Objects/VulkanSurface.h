#ifndef _TRAP_VULKANSURFACE_H_
#define _TRAP_VULKANSURFACE_H_

#include "Window/WindowingAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;
	class VulkanInstance;

	class VulkanSurface
	{
	public:
		VulkanSurface(TRAP::Ref<VulkanInstance> instance,
		              const TRAP::Ref<VulkanDevice>& device,
		              TRAP::Window* window);
		~VulkanSurface();

		VkSurfaceKHR& GetVkSurface();
		VkSurfaceCapabilitiesKHR GetVkSurfaceCapabilities() const;
		const std::vector<VkSurfaceFormatKHR>& GetVkSurfaceFormats() const;
		const std::vector<VkPresentModeKHR>& GetVkSurfacePresentModes() const;
		
	private:
		VkSurfaceKHR m_surface;

		VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
		std::vector<VkPresentModeKHR> m_surfacePresentModes;

		TRAP::Ref<VulkanInstance> m_instance;
	};
}

#endif /*_TRAP_VULKANSURFACE_H_*/