#ifndef TRAP_VULKANDEBUG_H
#define TRAP_VULKANDEBUG_H

#include "Core/Base.h"
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;

	class VulkanDebug final
	{
	public:
		/// @brief Constructor.
		/// @param instance Vulkan instance.
		explicit VulkanDebug(Ref<VulkanInstance> instance);
		/// @brief Destructor.
		~VulkanDebug();

		/// @brief Copy constructor.
		consteval VulkanDebug(const VulkanDebug&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanDebug& operator=(const VulkanDebug&) noexcept = delete;
		/// @brief Move constructor.
		VulkanDebug(VulkanDebug&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanDebug& operator=(VulkanDebug&&) noexcept = default;

	private:
		VkDebugUtilsMessengerEXT m_debugUtils = nullptr;
		VkDebugReportCallbackEXT m_debugReport = nullptr;

		Ref<VulkanInstance> m_instance;
	};
}

#endif /*TRAP_VULKANDEBUG_H*/
