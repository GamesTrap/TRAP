#ifndef _TRAP_VULKANCONTEXT_H_
#define _TRAP_VULKANCONTEXT_H_

#include "Graphics/API/Context.h"
#include "Window/WindowingAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanContext final : public Context
	{
	public:
		explicit VulkanContext(Window* window);
		~VulkanContext();
		VulkanContext(const VulkanContext&) = default;
		VulkanContext& operator=(const VulkanContext&) = default;
		VulkanContext(VulkanContext&&) = default;
		VulkanContext& operator=(VulkanContext&&) = default;

		void SetVSyncIntervalInternal(uint32_t interval) override;

		void UseInternal(Window* window) override;

		static bool IsVulkanCapable();

		static INTERNAL::WindowingAPI::InternalWindow* GetCurrentWindow();
		static std::string_view GetAppName();

	private:
		static PFN_vkEnumerateInstanceVersion s_vkEnumerateInstanceVersion;

		static INTERNAL::WindowingAPI::InternalWindow* s_currentWindow;
		static std::string_view s_appName;
	};
}

#endif /*_TRAP_VULKANCONTEXT_H_*/