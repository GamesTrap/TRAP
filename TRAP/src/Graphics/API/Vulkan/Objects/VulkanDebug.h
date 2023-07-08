#ifndef TRAP_VULKANDEBUG_H
#define TRAP_VULKANDEBUG_H

#include "Core/Base.h"
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;

	class VulkanDebug
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="instance">Vulkan instance.</param>
		explicit VulkanDebug(Ref<VulkanInstance> instance);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanDebug();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanDebug(const VulkanDebug&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanDebug& operator=(const VulkanDebug&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanDebug(VulkanDebug&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanDebug& operator=(VulkanDebug&&) noexcept = default;

	private:
		/// <summary>
		/// Callback for Vulkan debug utils messenger extension.
		/// </summary>
		/// <param name="messageSeverity">Severity of the message.</param>
		/// <param name="messageType">Type of message (unused).</param>
		/// <param name="callbackData">Message data.</param>
		/// <param name="userData">User data (unused).</param>
		/// <returns>VK_FALSE (as defined in Vulkan spec).</returns>
		static constexpr VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
																                 VkDebugUtilsMessageTypeFlagsEXT messageType,
																                 const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
																                 void* userData);
		/// <summary>
		/// Callback for Vulkan debug report messenger extension.
		/// </summary>
		/// <param name="flags">Severity of the message.</param>
		/// <param name="objectType">Object type (unused).</param>
		/// <param name="object">Object (unused).</param>
		/// <param name="location">Location (unused).</param>
		/// <param name="messageCode">Message code.</param>
		/// <param name="layerPrefix">Layer prefix.</param>
		/// <param name="message">Message data.</param>
		/// <param name="userData">User data (unused).</param>
		/// <returns>VK_FALSE (as defined in Vulkan spec).</returns>
		static constexpr VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReportCallback(VkDebugReportFlagsEXT flags,
																	              VkDebugReportObjectTypeEXT objectType,
																	              uint64_t object, size_t location,
																	              int32_t messageCode,
																	              std::string_view layerPrefix,
																	              std::string_view message, void* userData);
#ifdef ENABLE_DEBUG_UTILS_EXTENSION
		VkDebugUtilsMessengerEXT m_debugUtils = nullptr;
#else
		VkDebugReportCallbackEXT m_debugReport = nullptr;
#endif

		Ref<VulkanInstance> m_instance;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr VkBool32 TRAP::Graphics::API::VulkanDebug::VulkanDebugUtilsCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                              [[maybe_unused]] const VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                              const VkDebugUtilsMessengerCallbackDataEXT* const callbackData,
                                                                              [[maybe_unused]] void* userData)
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
																	           [[maybe_unused]] const uint64_t object,
																	           [[maybe_unused]] const size_t location,
																	           const int32_t messageCode,
																	           const std::string_view layerPrefix,
																	           const std::string_view message,
																	           [[maybe_unused]] void* const userData)
{
	std::string str = Log::RendererVulkanDebugPrefix;
	str.pop_back();

	if((flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0u)
		TP_INFO(str, '[', !layerPrefix.empty() ? layerPrefix : "", "] ", message, " (", messageCode, ')');
	if((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0u)
		TP_WARN(str, '[', !layerPrefix.empty() ? layerPrefix : "", "] ", message, " (", messageCode, ')');
	if((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0u)
		TP_ERROR(str, '[', !layerPrefix.empty() ? layerPrefix : "", "] ", message, " (", messageCode, ')');

	return VK_FALSE;
}

#endif /*TRAP_VULKANDEBUG_H*/