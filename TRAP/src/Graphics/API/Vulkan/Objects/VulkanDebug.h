#ifndef TRAP_VULKANDEBUG_H
#define TRAP_VULKANDEBUG_H

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
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
																  VkDebugUtilsMessageTypeFlagsEXT messageType,
																  const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
																  void* userData);

		VkDebugUtilsMessengerEXT m_debugReport;

		Ref<VulkanInstance> m_instance;
	};
}

#endif /*TRAP_VULKANDEBUG_H*/