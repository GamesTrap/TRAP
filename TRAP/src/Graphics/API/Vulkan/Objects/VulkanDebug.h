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
		/// @brief Callback for Vulkan debug utils messenger extension.
		/// @param messageSeverity Severity of the message.
		/// @param messageType Type of message (unused).
		/// @param callbackData Message data.
		/// @param userData User data (unused).
		/// @return VK_FALSE (as defined in Vulkan spec).
		static constexpr VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
																                 VkDebugUtilsMessageTypeFlagsEXT messageType,
																                 const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
																                 void* userData);
		/// @brief Callback for Vulkan debug report messenger extension.
		/// @param flags Severity of the message.
		/// @param objectType Object type (unused).
		/// @param object Object (unused).
		/// @param location Location (unused).
		/// @param messageCode Message code.
		/// @param layerPrefix Layer prefix.
		/// @param message Message data.
		/// @param userData User data (unused).
		/// @return VK_FALSE (as defined in Vulkan spec).
		static constexpr VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReportCallback(VkDebugReportFlagsEXT flags,
																	              VkDebugReportObjectTypeEXT objectType,
																	              u64 object, usize location,
																	              i32 messageCode,
																	              const char* layerPrefix,
																	              const char* message, void* userData);

		VkDebugUtilsMessengerEXT m_debugUtils = nullptr;
		VkDebugReportCallbackEXT m_debugReport = nullptr;

		Ref<VulkanInstance> m_instance;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkBool32 TRAP::Graphics::API::VulkanDebug::VulkanDebugUtilsCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                              [[maybe_unused]] const VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                              const VkDebugUtilsMessengerCallbackDataEXT* const callbackData,
                                                                              [[maybe_unused]] void* const userData)
{
	std::string str = Log::RendererVulkanDebugPrefix;
	str.pop_back();

	if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) != 0)
		TP_INFO(str, '[', callbackData->pMessageIdName != nullptr ? callbackData->pMessageIdName : "", "] ",
		        callbackData->pMessage, " (", callbackData->messageIdNumber, ')');
	else if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0)
		TP_WARN(str, '[', callbackData->pMessageIdName != nullptr ? callbackData->pMessageIdName : "", "] ",
		        callbackData->pMessage, " (", callbackData->messageIdNumber, ')');
	else if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0)
		TP_ERROR(str, '[', callbackData->pMessageIdName != nullptr ? callbackData->pMessageIdName : "", "] ",
		         callbackData->pMessage, " (", callbackData->messageIdNumber, ')');

	return VK_FALSE;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkBool32 TRAP::Graphics::API::VulkanDebug::VulkanDebugReportCallback(const VkDebugReportFlagsEXT flags,
																	           [[maybe_unused]] const VkDebugReportObjectTypeEXT objectType,
																	           [[maybe_unused]] const u64 object,
																	           [[maybe_unused]] const usize location,
																	           const i32 messageCode,
																	           const char* const layerPrefix,
																	           const char* const message,
																	           [[maybe_unused]] void* const userData)
{
	std::string str = Log::RendererVulkanDebugPrefix;
	str.pop_back();

	if((flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0u)
		TP_INFO(str, '[', layerPrefix != nullptr ? layerPrefix : "", "] ", message, " (", messageCode, ')');
	if((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0u)
		TP_WARN(str, '[', layerPrefix != nullptr ? layerPrefix : "", "] ", message, " (", messageCode, ')');
	if((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0u)
		TP_ERROR(str, '[', layerPrefix != nullptr ? layerPrefix : "", "] ", message, " (", messageCode, ')');

	return VK_FALSE;
}

#endif /*TRAP_VULKANDEBUG_H*/
