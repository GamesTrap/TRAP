#include "TRAPPCH.h"
#include "VulkanDebug.h"

#include "VulkanInits.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanDebug::VulkanDebug(Ref<VulkanInstance> instance)
	: m_debugReport(nullptr), m_instance(std::move(instance))
{
	TRAP_ASSERT(m_instance, "instance is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDebugPrefix, "Registering Debug Callback");
#endif

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	if(VulkanRenderer::s_debugUtilsExtension)
	{
		const VkDebugUtilsMessengerCreateInfoEXT info = VulkanInits::DebugUtilsMessengerCreateInfo(VulkanDebugUtilsCallback);
		const VkResult res = vkCreateDebugUtilsMessengerEXT(m_instance->GetVkInstance(), &info, nullptr, &m_debugUtils);
		if(res != VK_SUCCESS)
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Couldn't create Debug Utils Messenger!");
	}
#else
	if(VulkanRenderer::s_debugReportExtension)
	{
		const VkDebugReportCallbackCreateInfoEXT info = VulkanInits::DebugReportCallbackCreateInfo(VulkanDebugReportCallback);
		const VkResult res = vkCreateDebugReportCallbackEXT(m_instance->GetVkInstance(), &info, nullptr, &m_debugReport);
		if(res != VK_SUCCESS)
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Couldn't create Debug Report Messenger!");
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDebug::~VulkanDebug()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDebugPrefix, "Unregistering Debug Callback");
#endif

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	TRAP_ASSERT(m_debugUtils);
	vkDestroyDebugUtilsMessengerEXT(m_instance->GetVkInstance(), m_debugUtils, nullptr);
	m_debugUtils = nullptr;
#else
	TRAP_ASSERT(m_debugReport);
	vkDestroyDebugReportCallbackEXT(m_instance->GetVkInstance(), m_debugReport, nullptr);
	m_debugReport = nullptr;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

VkBool32 TRAP::Graphics::API::VulkanDebug::VulkanDebugUtilsCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                    const VkDebugUtilsMessageTypeFlagsEXT,
                                                                    const VkDebugUtilsMessengerCallbackDataEXT* const callbackData,
                                                                    void*)
{
	std::string str = Log::RendererVulkanDebugPrefix;
	str.pop_back();

	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		TP_INFO(str, '[', callbackData->pMessageIdName ? callbackData->pMessageIdName : "", "] ",
		        callbackData->pMessage, " (", callbackData->messageIdNumber, ')');
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		TP_WARN(str, '[', callbackData->pMessageIdName ? callbackData->pMessageIdName : "", "] ",
		        callbackData->pMessage, " (", callbackData->messageIdNumber, ')');
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		TP_ERROR(str, '[', callbackData->pMessageIdName ? callbackData->pMessageIdName : "", "] ",
		         callbackData->pMessage, " (", callbackData->messageIdNumber, ')');

	return VK_FALSE;
}

//-------------------------------------------------------------------------------------------------------------------//

VkBool32 TRAP::Graphics::API::VulkanDebug::VulkanDebugReportCallback(const VkDebugReportFlagsEXT flags,
																	 const VkDebugReportObjectTypeEXT /*objectType*/,
																	 const uint64_t /*object*/,
																	 const size_t /*location*/,
																	 const int32_t messageCode,
																	 const std::string_view layerPrefix,
																	 const std::string_view message, void* const /*userData*/)
{
	std::string str = Log::RendererVulkanDebugPrefix;
	str.pop_back();

	if(flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		TP_INFO(str, '[', !layerPrefix.empty() ? layerPrefix : "", "] ", message, " (", messageCode, ')');
	if(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		TP_WARN(str, '[', !layerPrefix.empty() ? layerPrefix : "", "] ", message, " (", messageCode, ')');
	if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		TP_ERROR(str, '[', !layerPrefix.empty() ? layerPrefix : "", "] ", message, " (", messageCode, ')');

	return VK_FALSE;
}