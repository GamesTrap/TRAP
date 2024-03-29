#ifndef TRAP_VULKANSURFACE_H
#define TRAP_VULKANSURFACE_H

#ifndef TRAP_HEADLESS_MODE

#include "Core/Base.h"
#include "Window/Window.h"
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"

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
		              const TRAP::Window* window);
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
		[[nodiscard]] constexpr VkSurfaceKHR GetVkSurface() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan surface capabilities.
		/// </summary>
		/// <returns>Vulkan surface capabilities.</returns>
		[[nodiscard]] constexpr VkSurfaceCapabilitiesKHR GetVkSurfaceCapabilities() const noexcept;
		/// <summary>
		/// Retrieve all formats supported by the surface.
		/// </summary>
		/// <returns>All formats supported by the surface.</returns>
		[[nodiscard]] constexpr const std::vector<VkSurfaceFormatKHR>& GetVkSurfaceFormats() const noexcept;
		/// <summary>
		/// Retrieve all present modes supported by the surface.
		/// </summary>
		/// <returns>All present modes supported by the surface.</returns>
		[[nodiscard]] constexpr const std::vector<VkPresentModeKHR>& GetVkSurfacePresentModes() const noexcept;

	private:
		VkSurfaceKHR m_surface = VK_NULL_HANDLE;

		VkSurfaceCapabilitiesKHR m_surfaceCapabilities{};
		std::vector<VkSurfaceFormatKHR> m_surfaceFormats{};
		std::vector<VkPresentModeKHR> m_surfacePresentModes{};

		TRAP::Ref<VulkanInstance> m_instance;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSurfaceKHR TRAP::Graphics::API::VulkanSurface::GetVkSurface() const noexcept
{
	return m_surface;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSurfaceCapabilitiesKHR TRAP::Graphics::API::VulkanSurface::GetVkSurfaceCapabilities() const noexcept
{
	return m_surfaceCapabilities;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<VkSurfaceFormatKHR>& TRAP::Graphics::API::VulkanSurface::GetVkSurfaceFormats() const noexcept
{
	return m_surfaceFormats;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<VkPresentModeKHR>& TRAP::Graphics::API::VulkanSurface::GetVkSurfacePresentModes() const noexcept
{
	return m_surfacePresentModes;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_VULKANSURFACE_H*/