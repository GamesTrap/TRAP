#pragma once

namespace TRAP::Graphics::API
{
	class VulkanContext final : public Context
	{
	public:
		VulkanContext(Window* window);
		~VulkanContext();
		VulkanContext(const VulkanContext&) = default;
		VulkanContext& operator=(const VulkanContext&) = default;
		VulkanContext(VulkanContext&&) = default;
		VulkanContext& operator=(VulkanContext&&) = default;

		void SetVSyncIntervalInternal(unsigned int interval) override;

		void Present(Window* window);

		static VulkanContext* Get() { return dynamic_cast<VulkanContext*>(s_Context.get()); }
		VkSurfaceKHR& GetSurface() { return m_surface; }
		Window* GetWindow() const { return m_window; }

		void InitSurface();
		void DeInitSurface();

		void SetupSwapchain();
		void InitSwapchain();
		void DeInitSwapchain();

		void InitImageViews();
		void DeInitImageViews();

		std::vector<VkSurfaceFormatKHR> GetAvailableSurfaceFormats(VkPhysicalDevice physicalDevice) const;
		std::vector<VkPresentModeKHR> GetAvailableSurfacePresentModes(VkPhysicalDevice physicalDevice) const;
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
