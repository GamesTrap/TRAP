#include "TRAPPCH.h"
#include "VulkanDebug.h"

#include "VulkanInits.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanDebug::VulkanDebug(Ref<VulkanInstance> instance)
	: m_debugUtils(nullptr), m_debugReport(nullptr), m_instance(std::move(instance))
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_instance, "VulkanDebug(): Instance is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDebugPrefix, "Registering Debug Callback");
#endif

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	if(VulkanRenderer::s_debugUtilsExtension)
	{
		const VkDebugUtilsMessengerCreateInfoEXT info = VulkanInits::DebugUtilsMessengerCreateInfo(VulkanDebugUtilsCallback);
		const VkResult res = vkCreateDebugUtilsMessengerEXT(m_instance->GetVkInstance(), &info, nullptr, &m_debugUtils);
		TRAP_ASSERT(m_debugUtils, "VulkanDebug(): Debug Utils Messenger is nullptr");
		if(res != VK_SUCCESS)
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Couldn't create Debug Utils Messenger!");
	}
#else
	if(VulkanRenderer::s_debugReportExtension)
	{
		const VkDebugReportCallbackCreateInfoEXT info = VulkanInits::DebugReportCallbackCreateInfo(VulkanDebugReportCallback);
		const VkResult res = vkCreateDebugReportCallbackEXT(m_instance->GetVkInstance(), &info, nullptr, &m_debugReport);
		TRAP_ASSERT(m_debugReport, "VulkanDebug(): Debug Report Messenger is nullptr");
		if(res != VK_SUCCESS)
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Couldn't create Debug Report Messenger!");
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDebug::~VulkanDebug()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDebugPrefix, "Unregistering Debug Callback");
#endif

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	vkDestroyDebugUtilsMessengerEXT(m_instance->GetVkInstance(), m_debugUtils, nullptr);
	m_debugUtils = nullptr;
#else
	vkDestroyDebugReportCallbackEXT(m_instance->GetVkInstance(), m_debugReport, nullptr);
	m_debugReport = nullptr;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

VkBool32 TRAP::Graphics::API::VulkanDebug::VulkanDebugUtilsCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

VkBool32 TRAP::Graphics::API::VulkanDebug::VulkanDebugReportCallback(const VkDebugReportFlagsEXT flags,
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