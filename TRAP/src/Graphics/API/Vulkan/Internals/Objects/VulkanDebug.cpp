#include "TRAPPCH.h"

#include "VulkanDebug.h"
#include "VulkanInstance.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::Vulkan::Debug::Debug(const Scope<Instance>& instance)
	: m_debugReport(nullptr), m_instance(instance.get())
{
	m_fvkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance->GetInstance(), "vkCreateDebugUtilsMessengerEXT"));
	m_fvkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance->GetInstance(), "vkDestroyDebugUtilsMessengerEXT"));

	if(m_fvkCreateDebugUtilsMessengerEXT == nullptr || m_fvkDestroyDebugUtilsMessengerEXT == nullptr)
	{
		TP_ERROR("[Renderer][Vulkan] Couldn't fetch debug function pointers!");
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT info = Initializers::DebugUtilsMessengerCreateInfo(VulkanDebugCallback);

	VkCall(m_fvkCreateDebugUtilsMessengerEXT(instance->GetInstance(), &info, nullptr, &m_debugReport));
	TRAP_RENDERER_ASSERT(m_debugReport, "[Renderer][Vulkan] Couldn't create Debug Utils Messenger!");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Debug::~Debug()
{
	if(m_debugReport && m_fvkDestroyDebugUtilsMessengerEXT)
	{
		m_fvkDestroyDebugUtilsMessengerEXT(m_instance->GetInstance(), m_debugReport, nullptr);
		m_debugReport = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VKAPI_ATTR VkBool32 TRAP::Graphics::API::Vulkan::Debug::VulkanDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData)
{
	std::ostringstream stream;

	stream << "[Renderer][Vulkan]";
	if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
		stream << "[Violation] ";
	else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
		stream << "[Performance] ";
	else
		stream << ' ';
	stream << callbackData->messageIdNumber << '(' << callbackData->pMessageIdName << ") " << callbackData->pMessage;
	if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		return false;
	if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		TP_INFO(stream.str());
	if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		TP_WARN(stream.str());
	if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		TP_ERROR(stream.str());

	return false;
}