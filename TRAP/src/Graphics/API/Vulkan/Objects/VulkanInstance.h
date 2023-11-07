#ifndef TRAP_VULKANINSTANCE_H
#define TRAP_VULKANINSTANCE_H

#include <string>
#include <string_view>
#include <vector>
#include <optional>

#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance
	{
	public:
		/// @brief Constructor.
		/// @param appName Application name.
		/// @param instanceLayers Instance layers to use.
		/// @param instanceExtensions Instance extensions to use.
		VulkanInstance(std::string_view appName, std::vector<std::string> instanceLayers,
		               std::vector<std::string> instanceExtensions);
		/// @brief Destructor.
		~VulkanInstance();

		/// @brief Copy constructor.
		constexpr VulkanInstance(const VulkanInstance&) noexcept = default;
		/// @brief Copy assignment operator.
		VulkanInstance& operator=(const VulkanInstance&) noexcept = default;
		/// @brief Move constructor.
		constexpr VulkanInstance(VulkanInstance&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanInstance& operator=(VulkanInstance&&) noexcept = default;

		/// @brief Retrieve the Vulkan instance handle.
		/// @return Vulkan instance handle.
		[[nodiscard]] constexpr VkInstance GetVkInstance() const noexcept;
		/// @brief Retrieve a list of used instance layers.
		/// @return List of used instance layers.
		[[nodiscard]] constexpr const std::vector<std::string>& GetUsedInstanceLayers() const noexcept;
		/// @brief Retrieve a list of used instance extensions.
		/// @return List of used instance extensions.
		[[nodiscard]] constexpr const std::vector<std::string>& GetUsedInstanceExtensions() const noexcept;

		/// @brief Retrieve the Vulkan instance version packed by VK_MAKE_API_VERSION.
		/// @return Vulkan instance version if available, empty optional otherwise.
		[[nodiscard]] static std::optional<u32> GetInstanceVersion();
		/// @brief Retrieve a list of available instance layers.
		/// @return List of available instance layers.
		[[nodiscard]] static const std::vector<VkLayerProperties>& GetAvailableInstanceLayers();
		/// @brief Retrieve a list of available instance extensions.
		/// @return List of available instance extensions.
		[[nodiscard]] static const std::vector<VkExtensionProperties>& GetAvailableInstanceExtensions();
		/// @brief Retrieve the properties for the given instance layer.
		/// @param instanceLayer Instance layer to get properties from.
		/// @return Instance layer properties.
		[[nodiscard]] static std::optional<VkLayerProperties> GetInstanceLayerProperties(std::string_view instanceLayer);
		/// @brief Retrieve the properties for the given instance extension.
		/// @param instanceExtension Instance extension to get properties from.
		/// @return Instance extension properties.
		[[nodiscard]] static std::optional<VkExtensionProperties> GetInstanceExtensionProperties(std::string_view instanceExtension);
		/// @brief Check whether an instance layer is supported or not.
		/// @param layer Instance layer to check.
		/// @return True if the instance layer is supported, false otherwise.
		[[nodiscard]] static bool IsLayerSupported(std::string_view layer);
		/// @brief Check whether an instance extension is supported or not.
		/// @param extension Instance extension to check.
		/// @return True if the instance extension is supported, false otherwise.
		[[nodiscard]] static bool IsExtensionSupported(std::string_view extension);

	private:
		/// @brief Load a list of available instance layers.
		static void LoadAllInstanceLayers();
		/// @brief Load a list of available instance extensions.
		static void LoadAllInstanceExtensions();

		inline constinit static std::optional<u32> s_instanceVersion = std::nullopt;

		VkInstance m_instance = VK_NULL_HANDLE;

		std::vector<std::string> m_instanceLayers{};
		std::vector<std::string> m_instanceExtensions{};

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