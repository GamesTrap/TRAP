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
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="instance">Vulkan instance.</param>
		/// <param name="device">Vulkan device.</param>
		/// <param name="window">Window to create surface for.</param>
		VulkanSurface(TRAP::Ref<VulkanInstance> instance,
		              const TRAP::Ref<VulkanDevice>& device,
		              TRAP::Window* window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanSurface();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanSurface(const VulkanSurface&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanSurface& operator=(const VulkanSurface&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanSurface(VulkanSurface&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanSurface& operator=(VulkanSurface&&) = default;

		/// <summary>
		/// Retrieve the Vulkan surface handle.
		/// </summary>
		/// <returns>Vulkan surface handle.</returns>
		VkSurfaceKHR GetVkSurface() const;
		/// <summary>
		/// Retrieve the Vulkan surface capabilities.
		/// </summary>
		/// <returns>Vulkan surface capabilities.</returns>
		VkSurfaceCapabilitiesKHR GetVkSurfaceCapabilities() const;
		/// <summary>
		/// Retrieve all formats supported by the surface.
		/// </summary>
		/// <returns>All formats supported by the surface.</returns>
		const std::vector<VkSurfaceFormatKHR>& GetVkSurfaceFormats() const;
		/// <summary>
		/// Retrieve all present modes supported by the surface.
		/// </summary>
		/// <returns>All present modes supported by the surface.</returns>
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