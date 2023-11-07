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

	/// @remark @headless This class is not available in headless mode.
	class VulkanSurface
	{
	public:
		/// @brief Constructor.
		/// @param instance Vulkan instance.
		/// @param device Vulkan device.
		/// @param window Window to create surface for.
		VulkanSurface(TRAP::Ref<VulkanInstance> instance,
		              const TRAP::Ref<VulkanDevice>& device,
		              const TRAP::Window* window);
		/// @brief Destructor.
		~VulkanSurface();

		/// @brief Copy constructor.
		VulkanSurface(const VulkanSurface&) noexcept = default;
		/// @brief Copy assignment operator.
		VulkanSurface& operator=(const VulkanSurface&) noexcept = default;
		/// @brief Move constructor.
		VulkanSurface(VulkanSurface&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanSurface& operator=(VulkanSurface&&) noexcept = default;

		/// @brief Retrieve the Vulkan surface handle.
		/// @return Vulkan surface handle.
		[[nodiscard]] constexpr VkSurfaceKHR GetVkSurface() const noexcept;
		/// @brief Retrieve the Vulkan surface capabilities.
		/// @return Vulkan surface capabilities.
		[[nodiscard]] constexpr VkSurfaceCapabilitiesKHR GetVkSurfaceCapabilities() const noexcept;
		/// @brief Retrieve all formats supported by the surface.
		/// @return All formats supported by the surface.
		[[nodiscard]] constexpr const std::vector<VkSurfaceFormatKHR>& GetVkSurfaceFormats() const noexcept;
		/// @brief Retrieve all present modes supported by the surface.
		/// @return All present modes supported by the surface.
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