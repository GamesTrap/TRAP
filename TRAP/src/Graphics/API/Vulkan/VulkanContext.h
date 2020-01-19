#ifndef _TRAP_VULKANCONTEXT_H_
#define _TRAP_VULKANCONTEXT_H_

#include "Graphics/API/Context.h"

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

		void Present(const Scope<Window>& window);
		void UseInternal(const Window* window) override;

		static VulkanContext* Get();
		VkSurfaceKHR& GetSurface();
		Window* GetWindow() const;

		void InitSurface();
		void DeInitSurface(VkInstance instance);

		void SetupSwapchain();
		void InitSwapchain();
		void DeInitSwapchain(VkDevice device);

		void InitImageViews();
		void DeInitImageViews(VkDevice device);

		std::vector<VkSurfaceFormatKHR> GetAvailableSurfaceFormats(VkPhysicalDevice physicalDevice) const;
		std::vector<VkPresentModeKHR> GetAvailableSurfacePresentModes(VkPhysicalDevice physicalDevice) const;
		
		VkExtent2D GetSwapchainExtent() const;

		VkFormat GetSwapchainImageFormat() const;
		
		static bool IsVulkanCapable();

	private:
		std::vector<VkSurfaceFormatKHR> GetAvailableSurfaceFormats() const;
		std::vector<VkPresentModeKHR> GetAvailableSurfacePresentModes() const;
		static VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
		static VkPresentModeKHR ChooseSwapchainSurfacePresentMode(const std::vector<VkPresentModeKHR>& availableSurfacePresentModes);
		VkExtent2D ChooseSwapchainExtent() const;

		std::vector<VkImageView> m_swapchainImageViews;
		std::vector<VkImage> m_swapchainImages;
		VkSwapchainKHR m_swapchain;
		VkExtent2D m_extent;
		VkSurfaceCapabilitiesKHR m_capabilities;
		VkSurfaceFormatKHR m_format;
		VkPresentModeKHR m_presentMode;
		VkSurfaceKHR m_surface;
		Window* m_window;

		bool m_vsync;
	};
}

#endif /*_TRAP_VULKANCONTEXT_H_*/