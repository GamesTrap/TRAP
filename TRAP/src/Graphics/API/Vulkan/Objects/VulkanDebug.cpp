#include "TRAPPCH.h"
#include "VulkanDebug.h"

#include "VulkanInits.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanDebug::VulkanDebug(Ref<VulkanInstance> instance)
	: m_debugReport(nullptr), m_instance(std::move(instance))
{
	TRAP_ASSERT(m_instance, "instance is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDebugPrefix, "Registering Debug Callback");
#endif

	VkDebugUtilsMessengerCreateInfoEXT info = VulkanInits::DebugUtilsMessengerCreateInfo(VulkanDebugCallback);

	VkCall(vkCreateDebugUtilsMessengerEXT(m_instance->GetVkInstance(), &info, nullptr, &m_debugReport));
	TRAP_ASSERT(m_debugReport, "[Renderer][Vulkan] Couldn't create Debug Utils Messenger!");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDebug::~VulkanDebug()
{
	TRAP_ASSERT(m_debugReport);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDebugPrefix, "Unregistering Debug Callback");
#endif

	vkDestroyDebugUtilsMessengerEXT(m_instance->GetVkInstance(), m_debugReport, nullptr);
	m_debugReport = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

VkBool32 TRAP::Graphics::API::VulkanDebug::VulkanDebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                               const VkDebugUtilsMessageTypeFlagsEXT,
                                                               const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
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