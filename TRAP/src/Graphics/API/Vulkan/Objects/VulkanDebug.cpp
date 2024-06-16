#include "TRAPPCH.h"
#include "VulkanDebug.h"

#include "VulkanInits.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

namespace
{
	constexpr VkBool32 VulkanDebugUtilsCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
												[[maybe_unused]] const VkDebugUtilsMessageTypeFlagsEXT messageType,
												const VkDebugUtilsMessengerCallbackDataEXT* const callbackData,
											    [[maybe_unused]] void* const userData)
	{
		std::string str = TRAP::Log::RendererVulkanDebugPrefix;
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

	constexpr VkBool32 VulkanDebugReportCallback(const VkDebugReportFlagsEXT flags,
												 [[maybe_unused]] const VkDebugReportObjectTypeEXT objectType,
												 [[maybe_unused]] const u64 object,
												 [[maybe_unused]] const usize location,
												 const i32 messageCode,
												 const char* const layerPrefix,
												 const char* const message,
												 [[maybe_unused]] void* const userData)
	{
		std::string str = TRAP::Log::RendererVulkanDebugPrefix;
		str.pop_back();

		if((flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0u)
			TP_INFO(str, '[', layerPrefix != nullptr ? layerPrefix : "", "] ", message, " (", messageCode, ')');
		if((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0u)
			TP_WARN(str, '[', layerPrefix != nullptr ? layerPrefix : "", "] ", message, " (", messageCode, ')');
		if((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0u)
			TP_ERROR(str, '[', layerPrefix != nullptr ? layerPrefix : "", "] ", message, " (", messageCode, ')');

		return VK_FALSE;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDebug::VulkanDebug(Ref<VulkanInstance> instance)
	: m_instance(std::move(instance))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_instance, "VulkanDebug(): Instance is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDebugPrefix, "Registering Debug Callback");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(VulkanInstance::IsExtensionSupported(VulkanInstanceExtension::DebugUtils))
	{
		const VkDebugUtilsMessengerCreateInfoEXT info = VulkanInits::DebugUtilsMessengerCreateInfo(VulkanDebugUtilsCallback);
		const VkResult res = vkCreateDebugUtilsMessengerEXT(m_instance->GetVkInstance(), &info, nullptr, &m_debugUtils);
		TRAP_ASSERT(m_debugUtils, "VulkanDebug(): Debug Utils Messenger is nullptr");
		if(res != VK_SUCCESS)
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Couldn't create Debug Utils Messenger!");
			VkCall(res);
		}
	}
	else if(VulkanInstance::IsExtensionSupported(VulkanInstanceExtension::DebugReport))
	{
		const VkDebugReportCallbackCreateInfoEXT info = VulkanInits::DebugReportCallbackCreateInfo(VulkanDebugReportCallback);
		const VkResult res = vkCreateDebugReportCallbackEXT(m_instance->GetVkInstance(), &info, nullptr, &m_debugReport);
		TRAP_ASSERT(m_debugReport, "VulkanDebug(): Debug Report Messenger is nullptr");
		if(res != VK_SUCCESS)
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Couldn't create Debug Report Messenger!");
			VkCall(res);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDebug::~VulkanDebug()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDebugPrefix, "Unregistering Debug Callback");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(m_debugUtils != VK_NULL_HANDLE)
		vkDestroyDebugUtilsMessengerEXT(m_instance->GetVkInstance(), m_debugUtils, nullptr);

	if(m_debugReport != VK_NULL_HANDLE)
		vkDestroyDebugReportCallbackEXT(m_instance->GetVkInstance(), m_debugReport, nullptr);
}
