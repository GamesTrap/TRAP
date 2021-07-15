#ifndef _TRAP_VULKANDEBUG_H_
#define _TRAP_VULKANDEBUG_H_

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

#endif /*_TRAP_VULKANDEBUG_H_*/