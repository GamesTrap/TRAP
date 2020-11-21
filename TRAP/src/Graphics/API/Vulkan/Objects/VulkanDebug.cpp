#include "TRAPPCH.h"
#include "VulkanDebug.h"

#include "VulkanInits.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanDebug::VulkanDebug(Ref<VulkanInstance> instance)
	: m_debugReport(nullptr), m_instance(std::move(instance))
{
	VkDebugUtilsMessengerCreateInfoEXT info = VulkanInits::DebugUtilsMessengerCreateInfo(VulkanDebugCallback);

	VkCall(vkCreateDebugUtilsMessengerEXT(m_instance->GetVkInstance(), &info, nullptr, &m_debugReport));
	TRAP_ASSERT(m_debugReport, "[Renderer][Vulkan] Couldn't create Debug Utils Messenger!");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDebug::~VulkanDebug()
{
	if(m_debugReport)
	{
		vkDestroyDebugUtilsMessengerEXT(m_instance->GetVkInstance(), m_debugReport, nullptr);
		m_debugReport = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkBool32 TRAP::Graphics::API::VulkanDebug::VulkanDebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                               const VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                               const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                               void* userData)
{
	std::string str = Log::RendererVulkanPrefix;
	str.pop_back();

	if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
		str += "[Violation] ";
	else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
		str += "[Performance] ";
	else
		str += ' ';

	str += std::to_string(callbackData->messageIdNumber) + "(" + callbackData->pMessageIdName + ") " + callbackData->pMessage;

	if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		TP_INFO(str);
	else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		TP_WARN(str);
	else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		TP_ERROR(str);
	else TP_TRACE(str);

	return false;
}