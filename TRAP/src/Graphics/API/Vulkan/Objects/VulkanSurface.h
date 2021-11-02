#ifndef TRAP_VULKANSURFACE_H
#define TRAP_VULKANSURFACE_H

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

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		VulkanSurface(const VulkanSurface&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VulkanSurface& operator=(const VulkanSurface&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VulkanSurface(VulkanSurface&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		VulkanSurface& operator=(VulkanSurface&&) = default;

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

#endif /*TRAP_VULKANSURFACE_H*/