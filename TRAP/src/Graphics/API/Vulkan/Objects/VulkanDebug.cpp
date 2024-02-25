#include "TRAPPCH.h"
#include "VulkanDebug.h"

#include "VulkanInits.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanDebug::VulkanDebug(Ref<VulkanInstance> instance)
	: m_instance(std::move(instance))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_instance, "VulkanDebug(): Instance is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDebugPrefix, "Registering Debug Callback");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(VulkanRenderer::s_debugUtilsExtension)
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
	else if(VulkanRenderer::s_debugReportExtension)
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
