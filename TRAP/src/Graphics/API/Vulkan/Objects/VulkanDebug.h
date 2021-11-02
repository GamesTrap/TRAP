#ifndef TRAP_VULKANDEBUG_H
#define TRAP_VULKANDEBUG_H

namespace TRAP::Graphics::API
{
	class VulkanInstance;

	class VulkanDebug
	{
	public:
		explicit VulkanDebug(Ref<VulkanInstance> instance);
		~VulkanDebug();

		VulkanDebug(const VulkanDebug&) = default;
		VulkanDebug& operator=(const VulkanDebug&) = default;
		VulkanDebug(VulkanDebug&&) = default;
		VulkanDebug& operator=(VulkanDebug&&) = default;

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
																  VkDebugUtilsMessageTypeFlagsEXT messageType,
																  const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
																  void* userData);

		VkDebugUtilsMessengerEXT m_debugReport;

		Ref<VulkanInstance> m_instance;
	};
}

#endif /*TRAP_VULKANDEBUG_H*/