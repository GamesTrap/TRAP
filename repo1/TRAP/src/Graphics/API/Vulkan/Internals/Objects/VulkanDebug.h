#ifndef _TRAP_VULKANDEBUG_H_
#define _TRAP_VULKANDEBUG_H_

namespace TRAP::Graphics::API::Vulkan
{
	class Instance;

	class Debug
	{
	public:
		explicit Debug(const Scope<Instance>& instance);
		~Debug();
		Debug(const Debug&) = default;
		Debug& operator=(const Debug&) = default;
		Debug(Debug&&) = default;
		Debug& operator=(Debug&&) = default;
		
	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		                                                          VkDebugUtilsMessageTypeFlagsEXT messageType,
		                                                          const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		                                                          void* userData);
		
		VkDebugUtilsMessengerEXT m_debugReport;

		PFN_vkCreateDebugUtilsMessengerEXT m_fvkCreateDebugUtilsMessengerEXT = nullptr;
		PFN_vkDestroyDebugUtilsMessengerEXT m_fvkDestroyDebugUtilsMessengerEXT = nullptr;

		Instance* m_instance;
	};
}

#endif