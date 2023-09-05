#ifndef TRAP_VULKANINSTANCE_H
#define TRAP_VULKANINSTANCE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="appName">Application name.</param>
		/// <param name="instanceLayers">Instance layers to use.</param>
		/// <param name="instanceExtensions">Instance extensions to use.</param>
		VulkanInstance(std::string_view appName, std::vector<std::string> instanceLayers,
		               std::vector<std::string> instanceExtensions);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanInstance();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr VulkanInstance(const VulkanInstance&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanInstance& operator=(const VulkanInstance&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr VulkanInstance(VulkanInstance&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanInstance& operator=(VulkanInstance&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan instance handle.
		/// </summary>
		/// <returns>Vulkan instance handle.</returns>
		[[nodiscard]] constexpr VkInstance GetVkInstance() const noexcept;
		/// <summary>
		/// Retrieve a list of used instance layers.
		/// </summary>
		/// <returns>List of used instance layers.</returns>
		[[nodiscard]] constexpr const std::vector<std::string>& GetUsedInstanceLayers() const noexcept;
		/// <summary>
		/// Retrieve a list of used instance extensions.
		/// </summary>
		/// <returns>List of used instance extensions.</returns>
		[[nodiscard]] constexpr const std::vector<std::string>& GetUsedInstanceExtensions() const noexcept;

		/// <summary>
		/// Retrieve the Vulkan instance version packed by VK_MAKE_API_VERSION.
		/// </summary>
		/// <returns>Vulkan instance version.</returns>
		[[nodiscard]] static uint32_t GetInstanceVersion();
		/// <summary>
		/// Retrieve a list of available instance layers.
		/// </summary>
		/// <returns>List of available instance layers.</returns>
		[[nodiscard]] static const std::vector<VkLayerProperties>& GetAvailableInstanceLayers();
		/// <summary>
		/// Retrieve a list of available instance extensions.
		/// </summary>
		/// <returns>List of available instance extensions.</returns>
		[[nodiscard]] static const std::vector<VkExtensionProperties>& GetAvailableInstanceExtensions();
		/// <summary>
		/// Retrieve the properties for the given instance layer.
		/// </summary>
		/// <param name="instanceLayer">Instance layer to get properties from.</param>
		/// <returns>Instance layer properties.</returns>
		[[nodiscard]] static std::optional<VkLayerProperties> GetInstanceLayerProperties(std::string_view instanceLayer);
		/// <summary>
		/// Retrieve the properties for the given instance extension.
		/// </summary>
		/// <param name="instanceLayer">Instance extension to get properties from.</param>
		/// <returns>Instance extension properties.</returns>
		[[nodiscard]] static std::optional<VkExtensionProperties> GetInstanceExtensionProperties(std::string_view instanceExtension);
		/// <summary>
		/// Check whether an instance layer is supported or not.
		/// </summary>
		/// <param name="layer">Instance layer to check.</param>
		/// <returns>True if the instance layer is supported, false otherwise.</returns>
		[[nodiscard]] static bool IsLayerSupported(std::string_view layer);
		/// <summary>
		/// Check whether an instance extension is supported or not.
		/// </summary>
		/// <param name="extension">Instance extension to check.</param>
		/// <returns>True if the instance extension is supported, false otherwise.</returns>
		[[nodiscard]] static bool IsExtensionSupported(std::string_view extension);

	private:
		/// <summary>
		/// Load a list of available instance layers.
		/// </summary>
		static void LoadAllInstanceLayers();
		/// <summary>
		/// Load a list of available instance extensions.
		/// </summary>
		static void LoadAllInstanceExtensions();

		inline constinit static uint32_t s_instanceVersion = 0;

		VkInstance m_instance;

		std::vector<std::string> m_instanceLayers;
		std::vector<std::string> m_instanceExtensions;

		inline static std::vector<VkLayerProperties> s_availableInstanceLayers{};
		inline static std::vector<VkExtensionProperties> s_availableInstanceExtensions{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkInstance TRAP::Graphics::API::VulkanInstance::GetVkInstance() const noexcept
{
	return m_instance;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<std::string>& TRAP::Graphics::API::VulkanInstance::GetUsedInstanceLayers() const noexcept
{
	return m_instanceLayers;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<std::string>& TRAP::Graphics::API::VulkanInstance::GetUsedInstanceExtensions() const noexcept
{
	return m_instanceExtensions;
}

#endif /*TRAP_VULKANINSTANCE_H*/