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
		VulkanDebug(const VulkanDebug&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanDebug& operator=(const VulkanDebug&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanDebug(VulkanDebug&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanDebug& operator=(VulkanDebug&&) = default;

	private:
		/// <summary>
		/// Callback for Vulkan debug utils messenger extension.
		/// </summary>
		/// <param name="messageSeverity">Severity of the message.</param>
		/// <param name="messageType">Type of message (unused).</param>
		/// <param name="callbackData">Message data.</param>
		/// <param name="userData">User data (unused).</param>
		/// <returns>VK_FALSE (as defined in Vulkan spec).</returns>
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReportCallback(VkDebugReportFlagsEXT flags,
																	    VkDebugReportObjectTypeEXT objectType,
																	    uint64_t object, size_t location,
																	    int32_t messageCode,
																	    std::string_view layerPrefix,
																	    std::string_view message, void* userData);

		VkDebugUtilsMessengerEXT m_debugUtils;
		VkDebugReportCallbackEXT m_debugReport;

		Ref<VulkanInstance> m_instance;
	};
}

#endif /*TRAP_VULKANDEBUG_H*/