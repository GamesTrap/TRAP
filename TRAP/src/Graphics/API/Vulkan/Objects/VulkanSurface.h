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
		              const TRAP::Window* const window);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanSurface();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanSurface(const VulkanSurface&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanSurface& operator=(const VulkanSurface&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanSurface(VulkanSurface&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanSurface& operator=(VulkanSurface&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan surface handle.
		/// </summary>
		/// <returns>Vulkan surface handle.</returns>
		[[nodiscard]] VkSurfaceKHR GetVkSurface() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan surface capabilities.
		/// </summary>
		/// <returns>Vulkan surface capabilities.</returns>
		[[nodiscard]] VkSurfaceCapabilitiesKHR GetVkSurfaceCapabilities() const noexcept;
		/// <summary>
		/// Retrieve all formats supported by the surface.
		/// </summary>
		/// <returns>All formats supported by the surface.</returns>
		[[nodiscard]] const std::vector<VkSurfaceFormatKHR>& GetVkSurfaceFormats() const noexcept;
		/// <summary>
		/// Retrieve all present modes supported by the surface.
		/// </summary>
		/// <returns>All present modes supported by the surface.</returns>
		[[nodiscard]] const std::vector<VkPresentModeKHR>& GetVkSurfacePresentModes() const noexcept;

	private:
		VkSurfaceKHR m_surface;

		VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
		std::vector<VkPresentModeKHR> m_surfacePresentModes;

		TRAP::Ref<VulkanInstance> m_instance;
	};
}

#endif /*TRAP_VULKANSURFACE_H*/